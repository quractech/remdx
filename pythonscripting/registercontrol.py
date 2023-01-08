
class Register():
    def __init__(self, regNumber=None, name=None, startBit=0, numberOfBits=1, value=0, resetValue=0):
        self.regNumber = regNumber
        self.name = name
        self.startBit = startBit
        self.numberOfBits = numberOfBits
        self.value = value
        self.resetValue = resetValue

    def setReg(self, value):
        self.value = value

    def getReg(self, name) -> int:
        return self.value


if __name__ == "__main__":

    reg = Register()
    reg.name = "id0"
    reg.numberOfBits = 8
    reg.startBit = 4
    reg.value = 1
    reg.setReg(10)
    print(f"regName = {reg.name}")
    print(f"regValue = {reg.value}")
    print(f"startBit = {reg.startBit}")
    print(f"numberOfBits = {reg.numberOfBits}")
