package compiler.yourcode.example;

import compiler.graph.Context;

/**
 * Created by xw on 15-10-26.
 */
public class SampleContext extends Context {
    private final String context;

    public SampleContext(String context) {
        this.context = context;
    }

    @Override
    public String toString() {
        return this.context;
    }
}
