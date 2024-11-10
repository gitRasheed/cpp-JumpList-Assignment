#include "JumpList.h"

#include <algorithm>
#include <stdexcept>

Node::Node(const string& s, Node* next, Node* jump, int gap) {
	data_ = s;
	next_ = next;
	jump_ = jump;
	gap_ = gap;
}

Node::~Node() {
	next_ = nullptr;
	jump_ = nullptr;
}


JumpList::JumpList() {
	head_ = nullptr;
}

// DO NOT CHANGE
// You do not need to understand the code here to complete the assignment
JumpList::JumpList(const int size, const string* arr) {

	const string s[] = {"a", "b", "blah", "c", "cat", "d", "etc", "ooo...", "x", "zzz"};
	const string* sp = (arr == nullptr) ? s : arr;

	Node** npp = new Node*[size];
	npp[size-1] = new Node(sp[size-1], nullptr, nullptr, 0);
	for(int i = size-2; i >= 0; i--)
		npp[i] = new Node(sp[i], npp[i+1], nullptr, 0);

	if (arr == nullptr) {

		if (size==1) npp[0]->gap_ = 1;
		else if (size==3) npp[0]->gap_ = 3;
 		else if (size==10) {
			npp[0]->jump_ = npp[5];
			npp[5]->jump_ = npp[8];
			npp[0]->gap_ = 5;
			npp[5]->gap_ = 3;
			npp[8]->gap_ = 2;
		}
		else throw std::invalid_argument("size must be 1, 3 or 10");

	}
	else {

		// jump node every MAX_GAP_SIZE
		int i;
		for(i=0; i < size-MAX_GAP_SIZE; i += MAX_GAP_SIZE) {
			npp[i]->jump_ = npp[i+MAX_GAP_SIZE];
			npp[i]->gap_ = MAX_GAP_SIZE;
		}
		npp[i]->gap_ = size % MAX_GAP_SIZE;
		if (npp[i]->gap_ == 0) npp[i]->gap_ = MAX_GAP_SIZE; // 0 need to become 5!

	}

	head_ = npp[0];
	delete [] npp; // note that the local array npp gets destroyed here but the nodes stay alive!

}

JumpList::~JumpList() {
	while (head_ != nullptr) {
		Node* toDelete = head_;
		head_ = head_->next_;
		delete toDelete;
	}
}


int JumpList::size() const {
	int count = 0;

	Node* current = head_;
	while (current != nullptr) {
		count++;
		current = current->next_;
	}
	return count;
}

// DONE FOR YOU, DO NOT CHANGE
bool JumpList::find(const string& s) const {

	if (head_ == nullptr) return false;

	// moving along the fast lane
	Node* tmp = head_;
	while(tmp->jump_ != nullptr && tmp->jump_->data_ < s)
		tmp = tmp->jump_;

	// tmp now points to the jump node at the start of the segment where s could be

	// now slow lane
	while(tmp != nullptr) {

		if (tmp->data_ == s) return true; // match
		else if (tmp->data_ > s) return false; // went past without finding s
		else tmp = tmp->next_;
	}

	return false; // end of list
}

string JumpList::get(int i) const {
	if (i < 0 || head_ == nullptr) return "";

	Node* current = head_;
	int currentIndex = 0;

	while (current -> jump_ != nullptr && currentIndex + current -> gap_ <= i) {
		currentIndex += current -> gap_;
		current = current -> jump_;
	}

	while (currentIndex < i && current -> next_ != nullptr) {
		current = current -> next_;
		currentIndex ++;
	}

	if (currentIndex != i) return "";

	return current -> data_;
}

string JumpList::print() const {
	string result;

	const Node* current = head_;
	if (current != nullptr) {
		result += current -> data_;
		current = current -> next_;
		while (current != nullptr) {
			result += " " + current -> data_;
			current = current -> next_;
		}
	}

	result += "\n";

	current = head_;
	if (current != nullptr) {
		result += current -> data_;
		current = current -> jump_;
		while (current != nullptr){
			result += " " + current -> data_;
			current = current -> jump_;
		}
	}

	result += "\n";

	current = head_;
	if (current != nullptr) {
		result += std::to_string(current -> gap_);
		current = current -> jump_;
		while (current != nullptr) {
			result += " " + std::to_string(current -> gap_);
			current = current -> jump_;
		}
	}

	return result;
}

string JumpList::prettyPrint() const {
	if (head_ == nullptr) {
		return "\n\n";
	}

	string mainLine, jumpLine, gapLine;

	// Build the main line showing all nodes
	buildMainLine(mainLine, head_);

	// Build jump connections and gap sizes
	buildJumpAndGapLines(mainLine, jumpLine, gapLine, head_);

	return mainLine + "\n" + jumpLine + "\n" + gapLine;
}

void JumpList::buildMainLine(string& mainLine, Node* current) const {
	// Add first node
	mainLine += current -> data_;
	current = current -> next_;

	// Add remaining nodes with arrows
	while (current != nullptr) {
		mainLine += " --> " + current -> data_;
		current = current -> next_;
	}
}

void JumpList::buildJumpAndGapLines(const string& mainLine, string& jumpLine,
								  string& gapLine, Node* current) const {
	// Initialize with first node
	jumpLine += current -> data_;
	gapLine += std::to_string(current -> gap_);

	Node* currentJump = current -> jump_;

	// Process each jump node
	while (currentJump != nullptr) {
		// Add space after previous jump node
		jumpLine += " ";

		// Fill with dashes up to 2 positions before current node
		while (jumpLine.length() < mainLine.find(currentJump -> data_) - 2) {
			jumpLine += "-";
		}

		// Add arrow end and node
		jumpLine += "> " + currentJump -> data_;

		// Align gap number under the jump node
		while (gapLine.length() < mainLine.find(currentJump -> data_)) {
			gapLine += " ";
		}
		gapLine += std::to_string(currentJump -> gap_);

		currentJump = currentJump -> jump_;
	}
}

bool JumpList::insert(const string& s) {
	if (find(s)) {
		return false;
	}

	Node* newNode = new Node(s, nullptr, nullptr, 0);

	if (head_ == nullptr) {
		head_ = newNode;
		head_ -> gap_ = 1;
		return true;
	}

	Node* previous = nullptr;
	Node* current = head_;

	while (current -> jump_ != nullptr && current -> data_ < s) {
		previous = current;
		current = current -> next_;
	}

	while (current != nullptr && current -> data_ < s) {
		previous = current;
		current = current -> next_;
	}

	if (previous == nullptr) {
		newNode -> next_ = head_;
		newNode -> gap_ = head_ -> gap_;
		newNode -> jump_ = head_ -> jump_;
		head_ = newNode;
	} else {
		newNode -> next_ = current;
		previous -> next_ = newNode;
	}

	updateSegmentSizes(head_);
	return true;
}

void JumpList::updateSegmentSizes(Node* jumpNode) {
	while (jumpNode != nullptr) {
		int segmentSize = calculateSegmentSize(jumpNode);

		if (segmentSize > MAX_GAP_SIZE) {
			splitSegment(jumpNode, segmentSize);
		} else {
			jumpNode -> gap_ = segmentSize;
		}

		jumpNode = jumpNode -> jump_;
	}
}

int JumpList::calculateSegmentSize(Node* jumpNode) {
	int size = 1;
	Node* temp = jumpNode -> next_;

	while (temp != jumpNode -> jump_ && temp != nullptr) {
		size++;
		temp = temp -> next_;
	}

	return size;
}

void JumpList::splitSegment(Node* jumpNode, int segmentSize) {
	int firstHalf = (segmentSize + 1) / 2;
	Node* newJump = jumpNode;

	for (int i = 0; i < firstHalf; i++) {
		newJump = newJump -> next_;
	}

	newJump -> jump_ = jumpNode -> jump_;
	jumpNode -> jump_ = newJump;
	newJump -> gap_ = segmentSize - firstHalf;
	jumpNode -> gap_ = firstHalf;
}

bool JumpList::eraseHead(Node* oldHead) {
    if (head_ -> next_ == nullptr) {
        delete head_;
        head_ = nullptr;
        return true;
    }

    head_ = head_ -> next_;
    // Make new head a jump node if not already
    if (oldHead -> jump_ != head_) {
        head_ -> jump_ = oldHead -> jump_;
        head_ -> gap_ = oldHead -> gap_ - 1;
    }
    if (head_ -> gap_ == 0) {
        head_ -> gap_ = 1;
    }
    delete oldHead;
    return true;
}

void JumpList::mergeSingleNodeSegment(Node* currentJump, Node* nodeToDelete) {
    currentJump -> jump_ = nodeToDelete -> jump_;
    currentJump -> gap_ = 1;

    Node* previous = currentJump;
    while (previous -> next_ != nodeToDelete) {
        previous = previous -> next_;
    }
    previous -> next_ = nodeToDelete -> next_;

    delete nodeToDelete;
}


bool JumpList::erase(const string& s) {
    if (head_ == nullptr) {
        return false;
    }

    // Handle head node deletion
    if (head_ -> data_ == s) {
        return eraseHead(head_);
    }

	// Find segment containing target using jump pointers
	Node* currentJump = head_;

	while (currentJump -> jump_ != nullptr && currentJump -> jump_ -> data_ <= s) {
		if (currentJump -> jump_ -> data_ == s) {
			Node* nodeToDelete = currentJump -> jump_;

			// Handle single-node segment case
			if (nodeToDelete -> gap_ == 1 && currentJump -> gap_ == 1) {
				mergeSingleNodeSegment(currentJump, nodeToDelete);
				return true;
			}

			// Handle normal segment merge
			int newGap = currentJump -> gap_ + nodeToDelete -> gap_ - 1;
			currentJump -> jump_ = nodeToDelete -> jump_;

			Node* previous = currentJump;
			while (previous -> next_ != nodeToDelete) {
				previous = previous -> next_;
			}
			previous -> next_ = nodeToDelete -> next_;

			delete nodeToDelete;

			// Split segment if too large
			if (newGap > MAX_GAP_SIZE) {
				splitLargeSegment(currentJump, newGap);
			} else {
				currentJump -> gap_ = newGap;
			}
			return true;
		}
		currentJump = currentJump -> jump_;
	}

    // Find and delete regular node in current segment
    Node* previous = currentJump;
    Node* current = currentJump -> next_;

    while (current != nullptr && current -> data_ != s) {
        if (current -> data_ > s) {
            return false;
        }
        previous = current;
        current = current -> next_;
    }

    if (current == nullptr) {
        return false;
    }

    previous -> next_ = current -> next_;
    currentJump -> gap_--;
    delete current;
    return true;
}