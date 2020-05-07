
def to_hex(value: int, length: int) -> str:
    hex_str = hex(value)
    if len(hex_str) - 2 < length:
        return "0" * (length - len(hex_str) + 2) + hex_str[2:]
    else:
        return hex_str[2:]


# Only work with standard dotted decimal ipv4 addresses
def ipv4_to_int(ipv4: str) -> int:
    index, cur, ret = 0, 0, 0
    while index < len(ipv4):
        if ipv4[index] != '.':
            cur *= 10
            cur += int(ipv4[index])
        else:
            ret <<= 8
            ret, cur = ret | cur, 0
        index += 1
    ret <<= 8
    ret, cur = ret | cur, 0
    return ret
