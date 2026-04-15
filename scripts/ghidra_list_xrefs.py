# @runtime Jython
# List references to a fixed Flash address (SPI1_TransferBytes @ 0x08005664)
# @category Analysis

from ghidra.program.model.symbol import RefType

TARGET = 0x08005664

def run():
    p = currentProgram
    af = p.getAddressFactory()
    addr = af.getDefaultAddressSpace().getAddress(TARGET)
    rm = p.getReferenceManager()
    refs = rm.getReferencesTo(addr)
    ref_list = []
    it = refs.iterator()
    while it.hasNext():
        ref_list.append(it.next())
    print("XREFS to {} (count={}):".format(addr, len(ref_list)))
    for ref in ref_list:
        print("  from {} type={}".format(ref.getFromAddress(), ref.getReferenceType()))

run()
