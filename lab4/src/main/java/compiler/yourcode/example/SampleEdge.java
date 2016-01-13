package compiler.yourcode.example;

import compiler.graph.Context;
import compiler.graph.Edge;
import compiler.graph.Node;

import java.util.HashSet;

/**
 * Created by xw on 15-10-26.
 */
public class SampleEdge extends Edge {

    public SampleEdge(Node src, Node dst) {
        this.contexts = new HashSet<Context>();
        this.src = src;
        this.dst = dst;
    }

    @Override
    public String toString() {
        return null;
    }
}
