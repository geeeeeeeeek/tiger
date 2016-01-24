package compiler.yourcode;

import java.io.InputStream;

/**
 * Created by Zhongyi on 1/13/16.
 */
public class InstructionInfo {
    public int start;
    public int codeUnits;
    public String instructionText;
    public Payload payload;

    public InstructionInfo(int start, int codeUnits, String instructionText) {
        this.start = start;
        this.codeUnits = codeUnits;
        this.instructionText = instructionText;
    }
}
