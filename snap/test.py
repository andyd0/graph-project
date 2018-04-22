import snap

# create a directed random graph on 100 nodes and 1k edges
G2 = snap.GenRndGnm(snap.PNGraph, 100, 1000)
# traverse the nodes
for NI in G2.Nodes():
    print "node id %d with out-degree %d and in-degree %d" % (
        NI.GetId(), NI.GetOutDeg(), NI.GetInDeg())
snap.SaveEdgeList(G2, "test.txt", "Save as tab-separated list of edges")
