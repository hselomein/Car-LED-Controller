def parse_lin_frame(data):
    #parse_lin_frame1(data)
    #return

    # Assuming data is a bytearray containing the LIN frame
    index = 0
    binaryString = ''.join(format(byte, '08b') for byte in data) # Get a binary string of the data
    print(f"Binary Data: {binaryString}")
    dataLength = len(data) -1 # prevent calculating length every time
    frames = []

    while len(binaryString) > 32: #skip last 4 bytes
        binaryString = binaryString[-(len(binaryString)-1):] # Shift data by 1 Bit
        #print(f"Binary Data: {binaryString}")
        
        # Sync Break: At least 13 dominant bits (0x00)
        if binaryString[:14] == '10000000000000':
            syncBreak = '10000000000000'
            binaryString = binaryString[-(len(binaryString)-14):] # Shift data by 12 Bits
            print(f"Break: {syncBreak}")

            # Sync Field: 0x55
            syncField = hex(int(binaryString[:8], 2)) # get hex value for sync
            print(f"Sync: {syncField}")
            if syncField == '0x55' :
                binaryString = binaryString[8:] # remove syncfield from data

                # Identifier: 1 byte
                identifier = int(binaryString[:8], 2)
                print(f"ID: {identifier}")
                binaryString = binaryString[8:]

                continue

                # Data Bytes: 0 to 8 bytes
                data_bytes = []
                sum_bytes = 0
                while index < dataLength and len(data_bytes) < 8:
                    data_bytes.append(data[index])
                    sum_bytes += dataCpy[index]
                    index += 1

                sum_bytes &= 0xFF
                sum_bytes = (~sum_bytes) & 0xFF

                # Checksum: 1 byte
                if index < dataLength:
                    checksum = dataCpy[index]
                    index += 1

                    # check checksum
                    csmatch = (sum_bytes == checksum)

                    # Store the parsed frame
                    frame = {
                        'sync_break': syncBreak,
                        'sync_field': syncField,
                        'identifier': identifier,
                        'data_bytes': data_bytes,
                        'checksum': checksum,
                        'cs_match': csmatch
                    }
                    frames.append(frame)
    return frames

def getBits(val, numBits=1, startBit=0):   # Get most significant bits
    #intVal = int.from_bytes(val[0:numBits//8+1], byteorder='big')
    if startBit > 0 :
        newVal = removeMSB(val, startBit)
    else :
        newVal = val
    return newVal >> (newVal.bit_length() - numBits)

def removeMSB(val, numBits=1): # remove most significant bits using mask and bitwise and operator
    newVal = val
    for i in range(numBits):
        mask = ~(1 << (newVal.bit_length() - 1))  # create a mask with all bits set except MSB
        newVal &=mask
    return newVal

def verify_checksum(frame, enhanced=True):
    if enhanced:
        checksum_data = [frame['identifier']] + frame['data_bytes']
    else:
        checksum_data = frame['data_bytes']
    
    calculated_checksum = (~sum(checksum_data)) & 0xFF
    is_valid = calculated_checksum == frame['checksum']
    return is_valid, calculated_checksum

def identify_frame_type(frame):
    # Master frame contains sync break, sync field, and identifier
    if frame['sync_field'] == 0x55 and frame['sync_break'] == 0x00:
        return "Master"
    else:
        return "Slave"

import argparse

def parse_lin_frame1(data):
    # Assuming data is a bytearray containing the LIN frame
    index = 0
    frames = []

    while index < len(data):
        # Sync Break: At least 13 dominant bits (0x00)
        if data[index] == 0x00:
            sync_break = data[index]
            index += 1
            while index < len(data) and data[index] == 0x00:
                index += 1

            # Sync Field: 0x55
            if index < len(data) and data[index] == 0x55:
                sync_field = data[index]
                index += 1

                # Identifier: 1 byte
                if index < len(data):
                    identifier = data[index]
                    index += 1

                    # Data Bytes: 0 to 8 bytes
                    data_bytes = []
                    sum_bytes = 0
                    while index < len(data) and len(data_bytes) < 8:
                        data_bytes.append(data[index])
                        sum_bytes += data[index]
                        index += 1

                    sum_bytes &= 0xFF
                    sum_bytes = (~sum_bytes) & 0xFF

                    # Checksum: 1 byte
                    if index < len(data):
                        checksum = data[index]
                        index += 1

                        # check checksum
                        csmatch = (sum_bytes == checksum)

                        # Store the parsed frame
                        frame = {
                            'sync_break': sync_break,
                            'sync_field': sync_field,
                            'identifier': identifier,
                            'data_bytes': data_bytes,
                            'checksum': checksum,
                            'cs_match': csmatch
                        }
                        frames.append(frame)
        else:
            index += 1

    return frames

def main():
    parser = argparse.ArgumentParser(description="Parse LIN bus data from a binary file.")
    parser.add_argument("file", help="The binary file to parse.")
    parser.add_argument("--enhanced", action="store_true", help="Use enhanced checksum calculation.")
    args = parser.parse_args()

    file_path = args.file
    with open(file_path, "rb") as file:
        binary_data = file.read()

    parsed_frames = parse_lin_frame(binary_data)

    for frame in parsed_frames:
        frame_type = identify_frame_type(frame)
        print(f"Frame Type: {frame_type}")
        print(f"Sync Break: {frame['sync_break']}")
        print(f"Sync Field: {frame['sync_field']}")
        print(f"Identifier: {frame['identifier']}")
        print(f"Data Bytes: {frame['data_bytes']}")
        print(f"Checksum: {frame['checksum']}")
        #is_valid_checksum, calculated_checksum = verify_checksum(frame, enhanced=args.enhanced)
        is_valid_checksum, calculated_checksum = verify_checksum(frame, False)
        print(f"Calculated Checksum: {calculated_checksum}")
        print(f"Checksum Valid: {is_valid_checksum}")
        is_valid_checksum, calculated_checksum = verify_checksum(frame, True)
        print(f"Calculated Enhanced Checksum: {calculated_checksum}")
        print(f"Enhanced Checksum Valid: {is_valid_checksum}")
        print("")

if __name__ == "__main__":
    main()