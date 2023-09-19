import os

def rwBinRange(outputPath, result):
    output_filename = os.path.join(outputPath, result.hash)
    if(os.path.exists(output_filename)):
        return f"File '{output_filename}' already exists."
    with open(output_filename, 'wb') as output_file:
        output_file.write(result.data)