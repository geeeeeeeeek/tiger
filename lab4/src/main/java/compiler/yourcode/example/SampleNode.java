package compiler.yourcode.example;

import compiler.graph.Edge;
import compiler.graph.Node;

import java.util.HashSet;

/**
 * Created by xw on 15-10-26.
 */
public class SampleNode extends Node{
    private final String name;

    public SampleNode(String name) {
        this.name = name;
        this.out = new HashSet<Edge>();
    }

    @Override
    public String toString() {
        return this.name;
    }
}
