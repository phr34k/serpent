import binascii

print "#include <string>"
print "unsigned char rawData[] = {"
with open('serpent.py', 'rb') as f:
    for chunk in iter(lambda: f.read(1), b''):
        print "0x" + chunk.encode('hex') + ","
print "};"
print "std::string data((char*)&rawData[0], sizeof(rawData));"