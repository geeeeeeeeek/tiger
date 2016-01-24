package compiler.yourcode;

/**
 * Created by Zhongyi on 1/14/16.
 */
public class Payload {
    public String type;
    public int[] offsets;

    public Payload(String type, int[] offsets) {
        this.type = type;
        this.offsets = offsets;
    }
}
