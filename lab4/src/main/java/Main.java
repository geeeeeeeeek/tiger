import compiler.dex.InstructionAnnotator;
import compiler.dex.InstructionFetcher;
import compiler.graph.*;
import compiler.yourcode.example.SampleContext;
import compiler.yourcode.example.SampleEdge;
import compiler.yourcode.example.SampleGraph;
import compiler.yourcode.example.SampleNode;
import org.jf.dexlib2.dexbacked.instruction.DexBackedInstruction;
import org.jf.dexlib2.iface.instruction.Instruction;

/**
 * Created by xw on 15-10-26.
 */
public class Main {
    public static void main(String[] args) {
        getInstructions();
        generateGraph();
    }

    /**
     * Example for getting instructions
     */
    private static void getInstructions(){
        Iterable<? extends Instruction> instructions = InstructionFetcher.fetch();
        System.out.println("Start\tCodeUnits\tInstruction");
        for (Instruction i : instructions){
            System.out.println(((DexBackedInstruction) i).instructionStart + "\t" + ((DexBackedInstruction) i).getCodeUnits()+ "\t"  + InstructionAnnotator.annotate(i));
        }
    }

    /**
     * Examples for generating a customized graph
     */
    private static void generateGraph(){
        Graph graph = new SampleGraph();
        Node a = new SampleNode("a := 3");
        Node b = new SampleNode("IF a > 0 :");
        Node c = new SampleNode("a := a - 1");
        graph.addNode(a);
        graph.addNode(b);
        graph.addNode(c);

        Edge e = new SampleEdge(a, b);
        Edge f = new SampleEdge(b, c);
        Edge g = new SampleEdge(c, b);
        Context context = new SampleContext("TRUE");
        f.addContext(context);
        a.addOutgoingEdge(e);
        b.addOutgoingEdge(f);
        c.addOutgoingEdge(g);

        GraphWriter.print(graph, "sample.dot");
    }
}
