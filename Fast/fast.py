def read_and_write_binary_range(input_filename, output_filename, start_index, end_index):
    try:
        with open(input_filename, 'rb') as input_file:
            content = input_file.read()
            if start_index < 0:
                start_index = 0
            if end_index > len(content):
                end_index = len(content)
            if start_index >= end_index:
                return "Invalid range."

            bytes_to_write = content[start_index:end_index]

        with open(output_filename, 'wb') as output_file:
            output_file.write(bytes_to_write)

        return f"Bytes from {start_index} to {end_index} written to {output_filename}"
    except FileNotFoundError:
        return f"File '{input_filename}' not found."
    except Exception as e:
        return f"An error occurred: {str(e)}"

from fastcdc import fastcdc
from hashlib import sha256

path = 'C:/Users/Raul/Desktop/asdf.jpg'

results = list(fastcdc(path, 16384, 32768, 65536, True, sha256))

idx = 0
for result in results:
    read_and_write_binary_range(path, './outputs/' + str(idx) + " - " + result.hash, result.offset, result.offset + result.length)
    idx += 1
