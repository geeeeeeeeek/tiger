package compiler.yourcode.impl;

import compiler.graph.Edge;
import compiler.graph.Node;
import compiler.yourcode.InstructionInfo;
import sun.awt.util.IdentityArrayList;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Created by xw on 15-10-26.
 */
public class CFGNode extends Node {
    private List<InstructionInfo> instructionInfos = new ArrayList<InstructionInfo>();
    public int start = 0, end = 0;
    private static final int BYTES_UNIT = 2;

    public CFGNode() {
        this.out = new HashSet<Edge>();
    }

    public CFGNode append(InstructionInfo info) {
        this.start = (this.start == 0) ? info.start : this.start;
        this.end = (this.end == 0) ? this.start + info.codeUnits * BYTES_UNIT : this.end + info.codeUnits * BYTES_UNIT;

        this.instructionInfos.add(info);

        return this;
    }

    public InstructionInfo getLastInstructionInfo() {
        return this.instructionInfos.get(this.instructionInfos.size() - 1);
    }

    public List<InstructionInfo> getInstructionInfos() {
        return this.instructionInfos;
    }

    public void setInstructionInfos(List<InstructionInfo> instructionInfos) {
        this.instructionInfos = instructionInfos;
        this.start = instructionInfos.get(0).start;
        this.end = instructionInfos.get(instructionInfos.size() - 1).start + instructionInfos.get(instructionInfos.size() - 1).codeUnits * BYTES_UNIT;
    }

    public Set<Edge> getOutgoingEdges() {
        return this.out;
    }

    public void setOutgoingEdges(Set<Edge> edges) {
        for (Edge e : edges) {
            CFGEdge edge = (CFGEdge) e;
            edge.setSrc(this);
        }
        this.out = edges;
    }
    public void addOutgoingEdge(Edge edge) {
        for (Edge e : this.out) {
            CFGEdge cfgEdge = (CFGEdge) e;
            if (cfgEdge.getDst() == edge.getDst()) {
                return;
            }
        }
        super.addOutgoingEdge(edge);
    }

    @Override
    public String toString() {
        String nodeContext = "";
        for (InstructionInfo info : this.instructionInfos) {
            nodeContext = nodeContext.concat(info.instructionText).concat("\n");
        }
        return nodeContext.trim();
    }
}
