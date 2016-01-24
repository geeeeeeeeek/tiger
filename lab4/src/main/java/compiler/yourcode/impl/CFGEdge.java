package compiler.yourcode.impl;

import compiler.graph.Context;
import compiler.graph.Edge;
import compiler.graph.Node;

import java.util.HashSet;

/**
 * Created by xw on 15-10-26.
 */
public class CFGEdge extends Edge {

    public CFGEdge(Node src, Node dst) {
        this.contexts = new HashSet<Context>();
        this.src = src;
        this.dst = dst;
    }

    public void setSrc(Node src) {
        this.src = src;
    }

    @Override
    public String toString() {
        return null;
    }
}
