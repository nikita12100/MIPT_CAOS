import aes256     

with open("data.aes", "rb") as f:
    encoded = f.read()

decoded = aes256.decode(encoded, "qwerty")
print(decoded)

aes256.set_default_password("qwerty")
print(aes256.decode(encoded))

try:
    aes256.decode(encoded, "wrong password")
except BaseException as e:
    print("Can't decode: {}".format(e))

