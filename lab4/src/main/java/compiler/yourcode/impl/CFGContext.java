package compiler.yourcode.impl;

import compiler.graph.Context;

/**
 * Created by xw on 15-10-26.
 */
public class CFGContext extends Context {
    private final String context;

    public CFGContext(String context) {
        this.context = context;
    }

    @Override
    public String toString() {
        return this.context;
    }
}
