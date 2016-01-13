package compiler.graph;

import java.util.Set;

/**
 * Created by xw on 15-10-26.
 */
public abstract class Edge {
    protected Set<Context> contexts;
    protected Node src;
    protected Node dst;

    public void addContext(Context context){
        this.contexts.add(context);
    }

    public Set<Context> getContexts() {
        return contexts;
    }

    public Node getSrc() {
        return src;
    }

    public Node getDst() {
        return dst;
    }
}
