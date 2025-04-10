def decode_instruction(hex_code):
    # Convert hex to binary
    binary_code = bin(int(hex_code, 16))[2:].zfill(32)
    
    # Extract fields
    opcode = int(binary_code[:6], 2)
    reg_a = int(binary_code[6:11], 2)
    reg_b = int(binary_code[11:16], 2)
    immediate = int(binary_code[16:], 2)
    reg_c = int(binary_code[16:21], 2)
    shift_value = int(binary_code[21:26], 2)
    function = int(binary_code[26:], 2)
    
    # I-Type Instructions
    i_type_instructions = {
        0:  f"BEQ: if(R[{reg_a}]==R[{reg_b}]) PC=PC+4+4*{immediate}",
        2:  f"L16: R[{reg_b}] = M[R[{reg_a}]+{immediate}]",
        16: f"L8U: R[{reg_b}] = M[R[{reg_a}]+{immediate}](7:0)",
        36: f"J: PC=4*{immediate}",
        48: f"S16: M[R[{reg_a}]+{immediate}] = R[{reg_b}]",
        50: f"S8: M[R[{reg_a}]+{immediate}] = R[{reg_b}](7:0)",
        54: f"ADDI: R[{reg_b}] = R[{reg_a}] + {immediate}",
        59: f"BNE: if(R[{reg_a}]!=R[{reg_b}]) PC=PC+4+4*{immediate}",
        61: f"JAL: R[31]=PC+4; PC=4*{immediate}",
    }
    
    # R-Type Instructions
    r_type_instructions = {
        0:  f"SUB: R[{reg_c}] = R[{reg_a}] - R[{reg_b}]",
        4:  f"OR: R[{reg_c}] = R[{reg_a}] | R[{reg_b}]",
        7:  f"NOR: R[{reg_c}] = ~(R[{reg_a}] | R[{reg_b}])",
        9:  f"ADD: R[{reg_c}] = R[{reg_a}] + R[{reg_b}]",
        11: f"SRA: R[{reg_c}] = (signed) R[{reg_b}] >> {shift_value}",
        19: f"XOR: R[{reg_c}] = R[{reg_a}] ^ R[{reg_b}]",
        24: f"AND: R[{reg_c}] = R[{reg_a}] & R[{reg_b}]",
        28: f"JR: PC=R[{reg_a}]",
        32: f"SLL: R[{reg_c}] = R[{reg_b}] << {shift_value}",
        35: f"SRL: R[{reg_c}] = (unsigned) R[{reg_b}] >> {shift_value}",
        36: f"SLT: R[{reg_c}] = (R[{reg_a}] < R[{reg_b}])",
    }
    
    if opcode == 62:  # R-Type
        return r_type_instructions.get(function, "Unknown R-Type Instruction")
    elif opcode in i_type_instructions:
        return i_type_instructions[opcode]
    else:
        return "Unknown Instruction"
    

def main():
    # Example usage
    while (True):
        hex_input = input("Enter 4-byte hex instruction or type quit: ")
        if (hex_input == "quit"):
            break
        else: 
            print(decode_instruction(hex_input))
    
main()