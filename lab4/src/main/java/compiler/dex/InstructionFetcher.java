package compiler.dex;

import org.jf.dexlib2.DexFileFactory;
import org.jf.dexlib2.dexbacked.DexBackedClassDef;
import org.jf.dexlib2.dexbacked.DexBackedMethod;
import org.jf.dexlib2.iface.instruction.Instruction;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Set;

/**
 * Created by xw on 15-10-26.
 */
public class InstructionFetcher {
    public static Iterable<? extends Instruction> fetch(){
        File file = null;
        try {
            file = stream2File(InstructionFetcher.class.getClassLoader().getResourceAsStream("test.apk"));
            Set<? extends DexBackedClassDef> classDefSet = DexFileFactory
                    .loadDexFile(file, 19).getClasses();
            for (DexBackedClassDef clz : classDefSet){
                if (clz.getType().contains("SimpleIntral")){
                    for (DexBackedMethod method : clz.getMethods()){
                        if (method.getName().contains("test")){
                            return method.getImplementation().getInstructions();
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    private static String prefix = "daf_temp_";
    private static int suffix = 0;

    public static File stream2File(InputStream in) throws IOException {
        suffix++;
        File result = File.createTempFile(prefix, "" + suffix);
        result.deleteOnExit();
        FileOutputStream out = new FileOutputStream(result);
        try {
            int read = 0;
            byte[] bytes = new byte[1024];

            while ((read = in.read(bytes)) != -1) {
                out.write(bytes, 0, read);
            }

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (out != null) {
                try {
                    // out.flush();
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        }
        return result;
    }
}
