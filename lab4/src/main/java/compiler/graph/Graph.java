package compiler.graph;

import java.util.Set;

/**
 * Created by xw on 15-10-26.
 */
public class Graph {
    protected Set<Node> nodes;
    public void addNode(Node node){
        this.nodes.add(node);
    }

    public Set<Node> getNodes() {
        return nodes;
    }
}
