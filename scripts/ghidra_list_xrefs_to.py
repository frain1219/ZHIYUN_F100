# @runtime Jython
# List references to a function by address
# @category Analysis

TARGET = 0x0800CB7A  # ADC1_AverageSamples

def run():
    p = currentProgram
    af = p.getAddressFactory()
    addr = af.getDefaultAddressSpace().getAddress(TARGET)
    rm = p.getReferenceManager()
    ref_list = []
    it = rm.getReferencesTo(addr).iterator()
    while it.hasNext():
        ref_list.append(it.next())
    print("XREFS to {} (count={}):".format(addr, len(ref_list)))
    for ref in ref_list:
        print("  from {} type={}".format(ref.getFromAddress(), ref.getReferenceType()))

run()
