package compiler.graph;

import java.util.Set;

/**
 * Created by xw on 15-10-26.
 */
public abstract class Node {
    protected Set<Edge> out;
    public abstract String toString();

    public void addOutgoingEdge(Edge edge){
        this.out.add(edge);
    }

    public Set<Edge> getOut() {
        return out;
    }
}
