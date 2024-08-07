#!/usr/bin/env python3

from PIL import Image
from sys import argv


# TurboVega-compatible packer
# Rewritten from C by Aleksandr Sharikhin(aka Nihirash)
class VdpPack:
    def compress(self, data):
        self.bit_position = 0
        self.bitbuffer = 0
        self.outdata = bytearray()
        self.string_data = [0] * 16
        self.string_size = 0
        self.string_read_index = 0
        self.string_write_index = 0
        self.string_data_index = 0
        self.window_size = 0
        self.window_index = 0
        self.window_data = [0] * 256

        self.outdata.extend(b'CmpT')
        self.outdata.extend(len(data).to_bytes(4, 'little'))

        for byte in data:
            if byte is not None: 
                self.compress_byte(byte)
        
        self.complete()

        return self.outdata

    ####### Just bitwise operations
    def put_bit(self, bit):
        self.bitbuffer = (self.bitbuffer << 1) | bit
        self.bit_position = self.bit_position + 1
        
        if self.bit_position >= 8:
            self.outdata.append(self.bitbuffer)
            self.bitbuffer = 0
            self.bit_position = 0

    def put_byte(self, b):
        for i in range(8):
            b = b & 0xff
            bit = b & 0x80
            if bit == 0:
                self.put_bit(0)
            else:
                self.put_bit(1)
            b = b << 1
    
    def complete(self):
        while self.string_size:
            self.put_bit(0)
            self.put_bit(0)
            self.put_byte(self.string_data[self.string_read_index])
            self.string_read_index = (self.string_read_index + 1) & 15
            self.string_size = self.string_size - 1

        if self.bit_position == 0:
            return
        
        for _ in range(8 - self.bit_position):
            self.put_bit(0)

    #### Packing itself

    def find_match(self, window):
        for start in range(self.window_size - window):
            wi = start
            si = self.string_read_index
            match = 1

            for _ in range(window):
                if self.window_data[wi] != self.string_data[si]:
                    match = 0
                    break
                wi = wi + 1
                si = si + 1
                wi = wi & 255
                si = si & 15
            if match:
                return start
            
        return -1


    def compress_byte(self, orig_byte):
        self.string_data[self.string_data_index] = orig_byte
        self.string_data_index = (self.string_data_index + 1) & 15

        if self.string_size < 16:
            self.string_size = self.string_size + 1
        else:
            self.string_read_index = (self.string_read_index + 1) & 15
        
        if self.string_size >= 16:
            if self.window_size >= 16:
                start = self.find_match(16)
                if start != -1:
                    self.put_bit(1)
                    self.put_bit(1)
                    self.put_byte(start)
                    self.string_size = 0
                    return

            if self.window_size >= 8:
                start = self.find_match(8)
                if start != -1:
                    self.put_bit(1)
                    self.put_bit(0)
                    self.put_byte(start)
                    self.string_size = self.string_size - 8
                    self.string_read_index = (self.string_read_index + 8) & 15
                    return 
                
            if self.window_size >= 4:
                start = self.find_match(4)
                if start != -1:
                    self.put_bit(0)
                    self.put_bit(1)
                    self.put_byte(start)
                    self.string_size = self.string_size - 4
                    self.string_read_index = (self.string_read_index + 4) & 15
                    return 

            old_byte = self.string_data[self.string_read_index]
            self.string_read_index = self.string_read_index + 1
            self.put_bit(0)
            self.put_bit(0)
            self.put_byte(old_byte)
            self.string_size = self.string_size - 1
            self.string_read_index = self.string_read_index & 15

            self.window_data[self.window_index] = old_byte
            self.window_index = (self.window_index + 1) & 255
            if self.window_size < 256:
                self.window_size = self.window_size + 1

def convert_bitmap(source_image):
    pixel_data = list(source_image.getdata())
    
    rgb2222_data = bytearray()
    for pixel in pixel_data:
        r, g, b, a = pixel
        pixel_value = ((r >> 6) & 0x03 | (((g >> 6) & 0x03) << 2) | (((b >> 6) & 0x03) << 4) | (((a >> 6) & 0x03) << 6))
        rgb2222_data.append(pixel_value)

    return rgb2222_data

## RLE packer by Aleksandr Sharikhin(aka Nihirash)
def pack_bitmap(bitmap):
    max_frame_size = 255
    frame_size = 0
    byte = -444

    data = bytearray()

    for read_byte in bitmap:
        if frame_size == 0:
            frame_size = 1
        else:
            if frame_size == max_frame_size or byte != read_byte:
                data.append(frame_size)
                data.append(byte)
                frame_size = 1
            else:
                frame_size = frame_size + 1
        byte = read_byte
    data.append(frame_size)
    data.append(byte)

    return data


def read_image_from_file(filename):
    image = Image.open(filename)
    image = image.convert('RGBA')
    image = image.point(lambda p: p // 85 * 85)
    
    return image

def convert_image(filename):
    image = read_image_from_file(filename)

    width, height = image.size

    bitmap = convert_bitmap(image)
    rle_ed_image = pack_bitmap(bitmap)
    lhz_ed_image = VdpPack().compress(bitmap)

    datum = bytearray()

    datum.extend(b'IM')

   # Is your image totally random noise? 
    if len(bitmap) < len(rle_ed_image) and len(bitmap) < len(lhz_ed_image):
        print("Using RAW data")
        datum.extend(len(bitmap).to_bytes(2, 'little'))
        datum.extend(width.to_bytes(2, 'little'))
        datum.extend(height.to_bytes(2, 'little'))
        datum.append(0)

        datum.extend(bitmap)
    # I dunno will be possible this or not
    elif len(rle_ed_image) < len(lhz_ed_image):
        print("Using RLE compression data")
        datum.extend(len(bitmap).to_bytes(2, 'little'))
        datum.extend(width.to_bytes(2, 'little'))
        datum.extend(height.to_bytes(2, 'little'))
        datum.append(1)

        datum.extend((len(rle_ed_image) // 2).to_bytes(2, 'little'))
        datum.extend(rle_ed_image)
    # Mostly common case I think
    else:
        print("Using TurboVega data compression")
        datum.extend(len(lhz_ed_image).to_bytes(2, 'little'))
        datum.extend(width.to_bytes(2, 'little'))
        datum.extend(height.to_bytes(2, 'little'))

        datum.append(2)
        datum.extend(lhz_ed_image)

    return datum

if __name__ == "__main__":
    print("Image to Agon's Buffered Image converter")
    app_name=argv[0]
    if len(argv) != 3:
        print("Usage: " + app_name + " <input-image> <output-file>")
    else:
        with open(argv[2], "wb") as f:
            f.write(convert_image(argv[1]))