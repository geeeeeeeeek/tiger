package compiler.dex;

import com.google.common.base.Joiner;
import com.google.common.collect.Lists;
import org.jf.dexlib2.VerificationError;
import org.jf.dexlib2.iface.instruction.*;
import org.jf.dexlib2.iface.instruction.formats.*;
import org.jf.dexlib2.util.ReferenceUtil;
import org.jf.util.NumberUtils;

import javax.annotation.Nonnull;
import java.util.List;

/**
 * Created by xw on 15-10-26.
 */
public class InstructionAnnotator {


    public static String annotate(@Nonnull Instruction instruction){

        String annotation = "Daf error!";
        switch (instruction.getOpcode().format) {
            case Format10x:
                annotation = annotateInstruction10x(instruction);
                break;
            case Format35c:
                annotation = annotateInstruction35c((Instruction35c)instruction);
                break;
            case Format3rc:
                annotation = annotateInstruction3rc((Instruction3rc)instruction);
                break;
            case ArrayPayload:
                annotation = annotateArrayPayload((ArrayPayload)instruction);
                break;
            case PackedSwitchPayload:
                annotation = annotatePackedSwitchPayload((PackedSwitchPayload)instruction);
                break;
            case SparseSwitchPayload:
                annotation = annotateSparseSwitchPayload((SparseSwitchPayload)instruction);
                break;
            default:
                annotation = annotateDefaultInstruction(instruction);
                break;
        }
        return annotation;
    }

    private static String formatRegister(int registerNum) {
        return String.format("v%d", registerNum);
    }

    private static String  annotateInstruction10x(@Nonnull Instruction instruction) {
        return instruction.getOpcode().name;
    }

    private static String  annotateInstruction35c(@Nonnull Instruction35c instruction) {
        List<String> args = Lists.newArrayList();

        int registerCount = instruction.getRegisterCount();
        if (registerCount == 1) {
            args.add(formatRegister(instruction.getRegisterC()));
        } else if (registerCount == 2) {
            args.add(formatRegister(instruction.getRegisterC()));
            args.add(formatRegister(instruction.getRegisterD()));
        } else if (registerCount == 3) {
            args.add(formatRegister(instruction.getRegisterC()));
            args.add(formatRegister(instruction.getRegisterD()));
            args.add(formatRegister(instruction.getRegisterE()));
        } else if (registerCount == 4) {
            args.add(formatRegister(instruction.getRegisterC()));
            args.add(formatRegister(instruction.getRegisterD()));
            args.add(formatRegister(instruction.getRegisterE()));
            args.add(formatRegister(instruction.getRegisterF()));
        } else if (registerCount == 5) {
            args.add(formatRegister(instruction.getRegisterC()));
            args.add(formatRegister(instruction.getRegisterD()));
            args.add(formatRegister(instruction.getRegisterE()));
            args.add(formatRegister(instruction.getRegisterF()));
            args.add(formatRegister(instruction.getRegisterG()));
        }

        String reference = ReferenceUtil.getReferenceString(instruction.getReference());

        return String.format("%s {%s}, %s",
                instruction.getOpcode().name, Joiner.on(", ").join(args), reference);
    }

    private static String annotateInstruction3rc(@Nonnull Instruction3rc instruction) {
        int startRegister = instruction.getStartRegister();
        int endRegister = startRegister + instruction.getRegisterCount() - 1;
        String reference = ReferenceUtil.getReferenceString(instruction.getReference());
        return String.format("%s {%s .. %s}, %s",
                instruction.getOpcode().name, formatRegister(startRegister), formatRegister(endRegister),
                reference);
    }

    private static String annotateDefaultInstruction(@Nonnull Instruction instruction) {
        List<String> args = Lists.newArrayList();

        if (instruction instanceof OneRegisterInstruction) {
            args.add(formatRegister(((OneRegisterInstruction)instruction).getRegisterA()));
            if (instruction instanceof TwoRegisterInstruction) {
                args.add(formatRegister(((TwoRegisterInstruction)instruction).getRegisterB()));
                if (instruction instanceof ThreeRegisterInstruction) {
                    args.add(formatRegister(((ThreeRegisterInstruction)instruction).getRegisterC()));
                }
            }
        }  else if (instruction instanceof VerificationErrorInstruction) {
            String verificationError = VerificationError.getVerificationErrorName(
                    ((VerificationErrorInstruction) instruction).getVerificationError());
            if (verificationError != null) {
                args.add(verificationError);
            } else {
                args.add("invalid verification error type");
            }
        }

        if (instruction instanceof ReferenceInstruction) {
            args.add(ReferenceUtil.getReferenceString(((ReferenceInstruction)instruction).getReference()));
        } else if (instruction instanceof OffsetInstruction) {
            int offset = ((OffsetInstruction)instruction).getCodeOffset();
            String sign = offset>=0?"+":"-";
            args.add(String.format("%s0x%x", sign, Math.abs(offset)));
        } else if (instruction instanceof NarrowLiteralInstruction) {
            int value = ((NarrowLiteralInstruction)instruction).getNarrowLiteral();
            if (NumberUtils.isLikelyFloat(value)) {
                args.add(String.format("%d # %f", value, Float.intBitsToFloat(value)));
            } else {
                args.add(String.format("%d", value));
            }
        } else if (instruction instanceof WideLiteralInstruction) {
            long value = ((WideLiteralInstruction)instruction).getWideLiteral();
            if (NumberUtils.isLikelyDouble(value)) {
                args.add(String.format("%d # %f", value, Double.longBitsToDouble(value)));
            } else {
                args.add(String.format("%d", value));
            }
        } else if (instruction instanceof FieldOffsetInstruction) {
            int fieldOffset = ((FieldOffsetInstruction)instruction).getFieldOffset();
            args.add(String.format("field@0x%x", fieldOffset));
        } else if (instruction instanceof VtableIndexInstruction) {
            int vtableIndex = ((VtableIndexInstruction)instruction).getVtableIndex();
            args.add(String.format("vtable@%d", vtableIndex));
        } else if (instruction instanceof InlineIndexInstruction) {
            int inlineIndex = ((InlineIndexInstruction)instruction).getInlineIndex();
            args.add(String.format("inline@%d", inlineIndex));
        }

        return String.format("%s %s",
                instruction.getOpcode().name, Joiner.on(", ").join(args));
    }
    /**
     * Only return instruction name.
     * @param instruction
     */
    private static String annotateArrayPayload(@Nonnull ArrayPayload instruction) {
        return instruction.getOpcode().name;
    }
    /**
     * Only return instruction name.
     * @param instruction
     */
    private static String  annotatePackedSwitchPayload(@Nonnull PackedSwitchPayload instruction) {
        return instruction.getOpcode().name;
    }
    /**
     * Only return instruction name.
     * @param instruction
     */
    private static String  annotateSparseSwitchPayload(@Nonnull SparseSwitchPayload instruction) {
        return instruction.getOpcode().name;
    }
}

