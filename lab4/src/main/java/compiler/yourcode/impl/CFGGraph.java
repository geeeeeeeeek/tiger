package compiler.yourcode.impl;

import compiler.graph.Graph;
import compiler.graph.Node;

import java.util.HashSet;

/**
 * Created by xw on 15-10-26.
 */
public class CFGGraph extends Graph {
    public CFGGraph() {
        this.nodes = new HashSet<Node>();
    }

    public CFGNode getCFGNodeByInstructionStart(int start) {
        for (Node node : this.nodes) {
            CFGNode cfgNode = (CFGNode) node;
            if (start >= cfgNode.start && start < cfgNode.end) {
                return cfgNode;
            }
        }
        return null;
    }
}
