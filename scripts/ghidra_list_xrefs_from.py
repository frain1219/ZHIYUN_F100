# @runtime Jython
# List references FROM a function entry (callers to this address)
# @category Analysis

TARGET = 0x0800cb7a  # FUN_0800cb7a ADC average sample

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
