#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H
#include <vector>

using namespace std;

/* Disjoint set implementation.
 * The nodes part of a set represent the parent by it's index, the root nodes are represented by a negative value.
 * The root nodes have two modes:
 * Rank:	Set root nodes are represented by the number of elements in the set, used during creation to track the set sizes.
 * Index:	Set root nodes are represented by the set index, allows the sets to be associated with a data structures when used.
 */
class DisjointSets
{
public:
	/* Initiate with the number of graph nodes. */
	DisjointSets();
	DisjointSets(int size);
	DisjointSets(const DisjointSets &orig);
	virtual ~DisjointSets();
	DisjointSets& operator=(const DisjointSets &orig);

	/* Connect two nodes and their related sets if they are not already connected. */
	void connect(int x, int y);
	/* Find the index of the root associated with the node at index. */
	int find(int index) const;
	/* Find the index of the root associated with the node at index. Compressing all searched nodes. */
	int findCompress(int index);

	/* Generate a record of a converted structure so negative values represent a set index identifier instead of rank. 
	min_rank	<<	Minimum rank required by a element for it to be given a set ID.
	*/
	void generateRecord(int min_rank);
	/* Restore structure to normal mode to allow further editing. */
	void rankMode();
	/* Apply the generated record, transforming the set so that root nodes represent index nodes.
	 * Ensure a record is generated first.
	*/
	void indexMode();
	/**/
	std::vector<std::vector<int>> getSets(int min_rank);

	/* Get the index of the set related to node at the index (ensure structure is in index mode!)
	 * Unqualified elments are represented by -1.
	*/
	int getSetIndex(int ind);
	/* Change an index representation to another number. */
	void renameIndex(int index, int new_name);
	
	/* Find largest rank of any set.*/
	int maxHeight() const;
	/* Get number of disjoint sets. */
	int numSet();
	/* Get number of disjoint sets with a minimum rank. Rank 0 represent the single element. */
	int numSet(int min_rank);

	/* Find index of the root associated with the node at index. */
	int operator[](unsigned int index) { return find(index); }

private:
	/* Set of parent index links for each graph node. */
	int* _links;
	/* Number of node links. */
	int _size;
	/* Number of disjoint sets. */
	int _num_set;
	/* Find the height of a node at index. */
	int height(int index) const;

	struct Record { int ind_link, rank, ind_id; };
	/* Struct containing rank information. */
	std::vector<Record> _record;
	/* Sets qualified for the record. */
	int _recorded_sets;

	/* Union two sets by their root node indices. */
	void unionRoot(int root1, int root2);


};

#endif