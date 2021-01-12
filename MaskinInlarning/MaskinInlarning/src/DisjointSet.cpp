#include"DisjointSet.h"
#include <algorithm>
#include <assert.h>

inline int toLinkIndex(int ind) { return ind >= 0 ? -2 - ind : -1; }
inline int fromLinkIndex(int ind) { return -2 - ind; } 

#pragma region Constructors, Destructor, Operators

DisjointSets::DisjointSets()
	: _links(NULL), _size(0), _num_set(0)
{	}
DisjointSets::DisjointSets(int size) 
 : _links(new int[size]), _size(size), _num_set(size) 
{
	for (int i = 0; i < _size; i++)
		_links[i] = -1;
}

DisjointSets::DisjointSets(const DisjointSets &orig)
	: _links(new int[orig._size]), _size(orig._size), _num_set(orig._num_set)
{
	for (int i = 0; i < _size; i++)
		_links[i] = orig._links[i];
}


DisjointSets& DisjointSets::operator=(const DisjointSets &orig){
	if (this == &orig)
		return *this;
	delete[] _links;

	this->_size = orig._size;
	this->_num_set = orig._num_set;
	this->_links = new int[_size];
	for (int i = 0; i < this->_size; i++)
		_links[i] = orig._links[i];
	return *this;
}


DisjointSets::~DisjointSets(){
	delete[] _links;
}

#pragma endregion

#pragma region Functions

int DisjointSets::find(int index) const{
	// Find compresses the node at index.
	int parent = _links[index];
	if (parent < 0) return index;
	int node;
	do {
		node = parent;
		parent = _links[parent];
	} while (parent >= 0);
	_links[index] = node;	// Compress the initial index
	return node;
}

int DisjointSets::findCompress(int index){
	// Find compressing all searched nodes
	if (_links[index] < 0)
		return index;

	int root = findCompress(_links[index]);
	_links[index] = root;
	return root;
}

/* Union two sets by their root node indices.
*/
void DisjointSets::unionRoot(int root1, int root2){
	// Union by rank
	if (root1 == root2)
		return; // Do not union on the same sets
	if (_links[root1] < _links[root2])
		_links[root2] = root1;
	else{
		_links[root2] -= (int)(_links[root1] == _links[root2]); // Rank increases by one if merged sets are of equal size.
		_links[root1] = root2;
	}
	_num_set--;
}

/* Connect two nodes and their related sets if they are not already connected. 
*/
void DisjointSets::connect(int x, int y) {
	unionRoot(find(x), find(y));
}

/* Find the tree rank of a certain node at index.
*/
int DisjointSets::height(int x) const{
	int parent = _links[x];
	int height = 1;
	while ((parent = _links[parent]) >= 0)
		height++;
	return height;
}

/* Get the index of the set related to node at the index (for current record). */
int DisjointSets::getSetIndex(int ind)
{
	assert(_record.size() > 0);
	return fromLinkIndex(_links[find(ind)]);
}

/* Change an index representation to another number. */
void DisjointSets:: renameIndex(int index, int new_name)
{
	assert(_record.size() > 0);
	_links[find(index)] = toLinkIndex(new_name);
}
std::vector<std::vector<int>> DisjointSets::getSets(int min_rank)
{
	generateRecord(min_rank);
	
	std::vector<std::vector<int>> sets(_recorded_sets);

	for (int i = 0; i < _size; i++)
	{
		int ind = getSetIndex(i);
		if (ind >= 0)
			sets[ind].push_back(i);
	}
	return sets;
}

int DisjointSets::maxHeight() const{
	
	int max_h = 0;
	for (int i = 0; i < _size; i++)
		max_h = std::max(height(i), max_h);
	return max_h;
}


/* Get number of disjoint sets. */
int DisjointSets::numSet() { return _num_set; }

/* Get number of disjoint sets with a minimum rank. Rank 0 represent the single element. */
int DisjointSets::numSet(int min_rank)
{ 
	// Links represent ranks with [-1, -MAX], min_rank uses [0,MAX]
	int count = 0;
	min_rank = -min_rank;
	for (int i = 0; i < _size; i++)
		count += (_links[i] < min_rank);
	return count;
}


/* Generate a record of a converted structure so negative values represent a set index identifier instead of rank. */
void DisjointSets::generateRecord(int min_rank)
{
	/* Generate a rank record for the sets of the specified rank. Note that records with rank less then specified are 
	 * represented by -1 which is then reserved.
	 */
	if (_record.size() > 0)
	{
		rankMode();
		_record.clear();
	}
	_record.reserve(_num_set);

	int num_set = 0;

	for (int i = 0; i < _size; i++)
	{
		if (_links[i] < 0)
		{
			// Links represent ranks with [-1, -MAX], min_rank uses [0,MAX]
			if (_links[i] < -min_rank)
			{
				_record.push_back({ i, _links[i], num_set });
				_links[i] = toLinkIndex(num_set);
				num_set++;
			}
			else // Represent non-qualifying sets as -1
			{ 
				_record.push_back({ i, _links[i], -1 });
				_links[i] = -1;
			}
		}
		
	}
	_recorded_sets = num_set;
}


/* Restore structure to normal mode. */
void DisjointSets::rankMode()
{
	assert(_record.size() > 0);
	for (unsigned int i = 0; i < _record.size(); i++)
		_links[_record[i].ind_link] = _record[i].rank;
}

/* Apply the generated record. */
void DisjointSets::indexMode()
{
	assert(_record.size() > 0);
	for (unsigned int i = 0; i < _record.size(); i++)
		_links[_record[i].ind_link] = toLinkIndex(_record[i].ind_id);
}
#pragma endregion


