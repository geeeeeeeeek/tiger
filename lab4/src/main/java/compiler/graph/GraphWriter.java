package compiler.graph;

import java.io.PrintWriter;

/**
 * Created by xw on 15-10-26.
 */
public class GraphWriter {
    public static void print(Graph g, String filePath){
        if (g == null || filePath == null){
            return;
        }

        PrintWriter out = null;
        try {
            out = new PrintWriter(filePath);
            StringBuilder sb= new StringBuilder();
            sb.append("digraph callGraph {\n");
            sb.append("\tnode [shape=rectangle]\n");
            for (Node node : g.nodes){
                sb.append("\t" + node.hashCode() + "  [label=\""
                        + node.toString().replace("\"", "\\\"") + "\"]\n");
            }
            for (Node node : g.nodes){
                for (Edge edge : node.out){
                    StringBuilder contextLabel = new StringBuilder();
                    for (Context context : edge.contexts){
                        contextLabel.append(context.toString() + "\n");
                    }
                    sb.append("\t" + edge.src.hashCode() + " -> "
                            + edge.dst.hashCode() + "[ label=\""
                            + contextLabel.toString().replace("\"", "\\\"") + "\" ]" + "\n");
                }
            }
            sb.append("}");
            out.print(sb);
            out.flush();
        } catch (Exception e){
            e.printStackTrace();
        } finally {
            if (out != null){
                out.close();
            }
        }
    }
}
