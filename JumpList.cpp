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
	string result;
	if (head_ == nullptr) {
		return "\n\n";
	}

	// First line - build first line and track jump node positions
	int pos = 0;
	Node* current = head_;

	// First node
	result += current -> data_;
	pos = current -> data_.length();
	string second_line = current -> data_;
	string third_line = std::to_string(current -> gap_);

	// Track the position of jump nodes for later lines
	Node* current_jump = head_ -> jump_;

	current = current -> next_;
	while (current != nullptr) {
		result += " --> ";
		pos += 5;
		result += current -> data_;
		pos += current -> data_.length();

		if (current == current_jump) {
			// Add a space first
			second_line += " ";

			// Fill with dashes up to 2 positions before the current node
			while (second_line.length() < result.find(current -> data_) - 2) {
				second_line += "-";
			}
			// Add arrow end and node
			second_line += "> " + current -> data_;

			// Add spaces and gap number in third line
			while (third_line.length() < result.find(current -> data_)) {
				third_line += " ";
			}
			third_line += std::to_string(current -> gap_);

			current_jump = current -> jump_;
		}
		current = current -> next_;
	}

	return result + "\n" + second_line + "\n" + third_line;
}

Node* JumpList::findInsertionSegment(const string& s) const {
	Node* current = head_;
	while (current -> jump_ != nullptr && current -> jump_ -> data_ < s) {
		current = current -> jump_;
	}
	return current;
}

Node* JumpList::findInsertionPoint(Node* segmentStart, const string& s) {
	Node* previous = segmentStart;
	Node* current = segmentStart -> next_;

	while (current != nullptr && current -> data_ < s) {
		previous = current;
		current = current -> next_;
	}

	return previous;
}

int JumpList::calculateSegmentSize(Node* jumpNode) {
	int size = 1;
	Node* temp = jumpNode->next_;
	while (temp != jumpNode->jump_ && temp != nullptr) {
		size++;
		temp = temp->next_;
	}
	return size;
}

void JumpList::splitSegmentIfNeeded(Node* jumpNode, int segmentSize) {
	if (segmentSize > MAX_GAP_SIZE) {
		int firstHalf = (segmentSize + 1) / 2;  // Make first half larger if odd

		// Find new jump node position
		Node* temp = jumpNode;
		for (int i = 0; i < firstHalf - 1; i++) {
			temp = temp->next_;
		}

		// Set up new jump node
		Node* newJumpNode = temp->next_;
		newJumpNode->jump_ = jumpNode->jump_;
		jumpNode->jump_ = newJumpNode;

		// Update gap sizes
		newJumpNode->gap_ = segmentSize - firstHalf;
		jumpNode->gap_ = firstHalf;
	} else {
		jumpNode->gap_ = segmentSize;
	}
}

void JumpList::updateSegmentStructure() {
	Node* jumpNode = head_;
	while (jumpNode != nullptr) {
		int segmentSize = calculateSegmentSize(jumpNode);
		splitSegmentIfNeeded(jumpNode, segmentSize);
		jumpNode = jumpNode->jump_;
	}
}

bool JumpList::insert(const string& s) {
	if (find(s)) return false;

	Node* newNode = new Node(s, nullptr, nullptr, 0);

	// Handle empty list
	if (head_ == nullptr) {
		head_ = newNode;
		head_->gap_ = 1;
		return true;
	}

	// Handle insertion before head
	if (s < head_->data_) {
		newNode->next_ = head_;
		newNode->gap_ = head_->gap_;
		newNode->jump_ = head_->jump_;
		head_ = newNode;
	} else {
		// Find correct segment and insertion point
		Node* segmentStart = findInsertionSegment(s);
		Node* insertPoint = findInsertionPoint(segmentStart, s);

		// Insert the new node
		newNode->next_ = insertPoint->next_;
		insertPoint->next_ = newNode;
	}

	// Update segments and handle splits
	updateSegmentStructure();

	return true;
}

// In JumpList.cpp:

// Find jump node whose next jump node contains target string
Node* JumpList::findJumpNodeWithNext(const string& s) const {
	Node* currJump = head_;
	while (currJump->jump_ != nullptr && currJump->jump_->data_ <= s) {
		if (currJump->jump_->data_ == s) {
			return currJump;
		}
		currJump = currJump->jump_;
	}
	return currJump;
}

// Handle merging segments and potential splitting after deleting a jump node
void JumpList::mergeAndSplitSegments(Node* currJump, Node* toDelete) {
	// Special handling for size-1 segments
	if (toDelete->gap_ == 1 && currJump->gap_ == 1) {
		currJump->jump_ = toDelete->jump_;
		currJump->gap_ = 1;  // Keep gap as 1 for remaining segment

		// Update next pointers
		Node* prev = currJump;
		while (prev->next_ != toDelete) {
			prev = prev->next_;
		}
		prev->next_ = toDelete->next_;
	} else {
		// Normal merge for larger segments
		int newGap = currJump->gap_ + toDelete->gap_ - 1;
		currJump->jump_ = toDelete->jump_;

		// Update next pointers
		Node* prev = currJump;
		while (prev->next_ != toDelete) {
			prev = prev->next_;
		}
		prev->next_ = toDelete->next_;

		// Check if split needed
		if (newGap > MAX_GAP_SIZE) {
			int firstHalf = (newGap + 1) / 2;
			Node* newJump = currJump;
			for (int i = 0; i < firstHalf; i++) {
				newJump = newJump->next_;
			}
			newJump->jump_ = currJump->jump_;
			currJump->jump_ = newJump;
			newJump->gap_ = newGap - firstHalf;
			currJump->gap_ = firstHalf;
		} else {
			currJump->gap_ = newGap;
		}
	}
}

// Find a non-jump node within a segment
Node* JumpList::findNodeInSegment(Node* segmentStart, const string& s) {
	Node* prev = segmentStart;
	Node* curr = segmentStart->next_;

	while (curr != nullptr && curr->data_ != s) {
		if (curr->data_ > s) return nullptr;
		prev = curr;
		curr = curr->next_;
	}

	return curr;
}

bool JumpList::erase(const string& s) {
	if (head_ == nullptr) return false;

	// Handle head case
	if (head_->data_ == s) {
		Node* oldHead = head_;
		if (head_->next_ == nullptr) {
			delete head_;
			head_ = nullptr;
		} else {
			head_ = head_->next_;
			// Make new head a jump node if it isn't already
			if (oldHead->jump_ != head_) {
				head_->jump_ = oldHead->jump_;
				head_->gap_ = oldHead->gap_ - 1;
			}
			// Special case for size-1 segments
			if (head_->gap_ == 0) head_->gap_ = 1;
			delete oldHead;
		}
		return true;
	}

	// Try to find s in a jump node
	Node* currJump = findJumpNodeWithNext(s);
	if (currJump->jump_ != nullptr && currJump->jump_->data_ == s) {
		Node* toDelete = currJump->jump_;
		mergeAndSplitSegments(currJump, toDelete);
		delete toDelete;
		return true;
	}

	// Try to find s in the current segment
	Node* nodeToDelete = findNodeInSegment(currJump, s);
	if (nodeToDelete == nullptr) return false;

	// Delete non-jump node and update the segment
	Node* prev = currJump;
	while (prev->next_ != nodeToDelete) {
		prev = prev->next_;
	}
	prev->next_ = nodeToDelete->next_;
	currJump->gap_--;
	delete nodeToDelete;
	return true;
}