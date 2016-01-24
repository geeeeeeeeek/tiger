package compiler.yourcode;

import compiler.dex.InstructionAnnotator;
import compiler.dex.InstructionFetcher;
import compiler.graph.Edge;
import compiler.graph.Graph;
import compiler.graph.GraphWriter;
import compiler.graph.Node;
import compiler.yourcode.impl.CFGEdge;
import compiler.yourcode.impl.CFGGraph;
import compiler.yourcode.impl.CFGNode;
import org.jf.dexlib2.Opcode;
import org.jf.dexlib2.dexbacked.instruction.DexBackedInstruction;
import org.jf.dexlib2.iface.instruction.Instruction;

import java.util.*;

/**
 * Created by Zhongyi on 1/13/16.
 */
public class CFGAnalyser {
    public List<String> jmpList = new ArrayList<String>(),
            cjmpList = new ArrayList<String>(),
            skipList = new ArrayList<String>();

    private CFGGraph graph = new CFGGraph();
    private List<Object> graphNodesArray = new ArrayList<Object>();
    private HashMap<Integer, Payload> payloadMap = new HashMap<Integer, Payload>();

    public static void main(String[] args) {
        CFGAnalyser analyser = new CFGAnalyser();
        // Jump rules
        analyser.jmpList.add("goto");

        // Conditional jump rules
        analyser.cjmpList.add("if-ge");
        analyser.cjmpList.add("if-eq");
        analyser.cjmpList.add("if-lez");
        analyser.cjmpList.add("packed-switch");

        // Placeholders rules
        analyser.skipList.add("nop");
        analyser.skipList.add("packed-switch-payload");
        analyser.skipList.add("array-payload");

        // Get instructions
        List<InstructionInfo> instructionInfos = analyser.getInstructionInfos();

        analyser.constructCFGGraph(instructionInfos);
        GraphWriter.print(analyser.graph, "cfg.dot");
        System.out.println(">> Output cfg.dot.");

        analyser.addDataDependencyOnGraph();
        GraphWriter.print(analyser.graph, "dpg.dot");
        System.out.println(">> Output dpg.dot.");
    }

    /**
     * Example for getting instructions
     */
    private List<InstructionInfo> getInstructionInfos() {
        Iterable<? extends Instruction> instructions = InstructionFetcher.fetch();

        List<InstructionInfo> instructionInfos = new ArrayList<InstructionInfo>();
        for (Instruction i : instructions) {
            InstructionInfo instructionInfo = new InstructionInfo(((DexBackedInstruction) i).instructionStart,
                    ((DexBackedInstruction) i).getCodeUnits(), InstructionAnnotator.annotate(i));
            if (i.getOpcode() == Opcode.PACKED_SWITCH_PAYLOAD) {
                int size = ((DexBackedInstruction) i).dexFile.readUshort(((DexBackedInstruction) i).instructionStart + 2);
                int[] offsetArray = new int[size];
                for (int index = 0; index < size; index++) {
                    int offsetInDex = ((DexBackedInstruction) i).instructionStart + 8 + index * 4;
                    offsetArray[index] = ((DexBackedInstruction) i).dexFile.readInt(offsetInDex);
                }
                Payload packedSwitchPayload = new Payload("packed-switch-payload", offsetArray);
                payloadMap.put(((DexBackedInstruction) i).instructionStart, packedSwitchPayload);
            }
            instructionInfos.add(instructionInfo);
        }
        return instructionInfos;
    }

    private void constructCFGGraph(List<InstructionInfo> instructionInfos) {
        CFGNode lastNode = null;

        // Start with a naive CFG, which split into basic blocks
        for (InstructionInfo info : instructionInfos) {
            String[] splitInstructionInfo = splitInstructionInfo(info);

            if (this.skipList.contains(splitInstructionInfo[0])) continue;
            if (lastNode != null) {
                lastNode.append(info);
                if (this.jmpList.contains(splitInstructionInfo[0])) {
                    CFGNode currentNode = new CFGNode();
                    graph.addNode(currentNode);
                    lastNode = currentNode;
                } else if (this.cjmpList.contains(splitInstructionInfo[0])) {
                    CFGNode currentNode = new CFGNode();
                    graph.addNode(currentNode);

                    CFGEdge edge = new CFGEdge(lastNode, currentNode);
                    lastNode.addOutgoingEdge(edge);
                    lastNode = currentNode;
                }
            } else {
                CFGNode currentNode = new CFGNode().append(info);
                graph.addNode(currentNode);
                lastNode = currentNode;
            }
        }

        // Split current nodes according to jmp statements
        Set<Node> graphNodes = graph.getNodes();
        Object[] array = graphNodes.toArray();
        while (array.length > 0) {
            for (Object object : array) {
                CFGNode cfgNode = (CFGNode) object;
                InstructionInfo info = cfgNode.getLastInstructionInfo();

                String[] splitInstructionInfo = splitInstructionInfo(info);
                if (splitInstructionInfo[0].equals("packed-switch")) {
                    int payloadOffset = Integer.decode(splitInstructionInfo[splitInstructionInfo.length - 1]);
                    Payload packedSwitchPayload = payloadMap.get(info.start + payloadOffset * 2);

                    if (packedSwitchPayload != null && packedSwitchPayload.type.equals("packed-switch-payload")) {
                        for (int index = 0; index < packedSwitchPayload.offsets.length; index++) {
                            int offset = packedSwitchPayload.offsets[index];
                            int instructionStart = info.start + offset * 2;
                            CFGNode jmpToCFGNode = this.splitNode(instructionStart,
                                    graph.getCFGNodeByInstructionStart(instructionStart));

                            CFGEdge edge = new CFGEdge(cfgNode, jmpToCFGNode);
                            cfgNode.addOutgoingEdge(edge);
                        }
                    }
                    continue;
                }

                if (this.jmpList.contains(splitInstructionInfo[0]) || this.cjmpList.contains(splitInstructionInfo[0])) {
                    int offset = Integer.decode(splitInstructionInfo[splitInstructionInfo.length - 1]);
                    int instructionStart = info.start + offset * 2;

                    CFGNode jmpToCFGNode = this.splitNode(instructionStart,
                            graph.getCFGNodeByInstructionStart(instructionStart));

                    CFGEdge edge = new CFGEdge(cfgNode, jmpToCFGNode);
                    cfgNode.addOutgoingEdge(edge);
                }
            }
            array = graphNodesArray.toArray();
            graphNodesArray = new ArrayList<Object>();
        }
    }

    private void addDataDependencyOnGraph() {
    }

    private String[] splitInstructionInfo(InstructionInfo info) {
        return info.instructionText.split("\\s|,\\s");
    }

    private CFGNode splitNode(int instructionStart, CFGNode node) {
        if (node.start == instructionStart) return node;

        List<InstructionInfo> instructionInfos = node.getInstructionInfos();
        for (InstructionInfo info : instructionInfos) {
            if (info.start != instructionStart) continue;

            CFGNode nextNode = new CFGNode();

            int index = instructionInfos.indexOf(info);

            nextNode.setInstructionInfos(instructionInfos.subList(index, instructionInfos.size()));
            nextNode.setOutgoingEdges(node.getOutgoingEdges());

            node.setInstructionInfos(instructionInfos.subList(0, index));
            node.setOutgoingEdges(new HashSet<Edge>());
            node.addOutgoingEdge(new CFGEdge(node, nextNode));

            graph.addNode(nextNode);
            graphNodesArray.add(nextNode);

            return nextNode;
        }
        return null;
    }
}
