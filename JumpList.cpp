#include "JumpList.h"
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
JumpList::JumpList(int size, const string* arr) {

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
		count += current->gap_;
		current = current->jump_;
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

	while (current->jump_ != nullptr && currentIndex + current->gap_ <= i) {
		currentIndex += current->gap_;
		current = current->jump_;
	}

	while (currentIndex < i && current->next_ != nullptr) {
		current = current->next_;
		currentIndex ++;
	}

	if (currentIndex != i) return "";

	return current->data_;
}

string JumpList::print() const {
	string result;

	const Node* current = head_;
	if (current != nullptr) {
		result += current->data_;
		current = current->next_;
		while (current != nullptr) {
			result += " " + current->data_;
			current = current->next_;
		}
	}

	result += "\n";

	current = head_;
	if (current != nullptr) {
		result += current->data_;
		current = current->jump_;
		while (current != nullptr){
			result += " " + current->data_;
			current = current->jump_;
		}
	}

	result += "\n";

	current = head_;
	if (current != nullptr) {
		result += std::to_string(current->gap_);
		current = current->jump_;
		while (current != nullptr) {
			result += " " + std::to_string(current->gap_);
			current = current->jump_;
		}
	}

	return result;
}

string JumpList::prettyPrint() const {
	string result;
	if (head_ == nullptr) {
		return "\n\n";
	}

	int pos = 0;
	Node* current = head_;

	result += current->data_;
	pos = current->data_.length();
	string second_line = current->data_;
	string third_line = std::to_string(current->gap_);

	Node* current_jump = head_->jump_;

	current = current->next_;
	while (current != nullptr) {
		result += " --> ";
		pos += 5;
		result += current->data_;
		pos += current->data_.length();

		if (current == current_jump) {
			second_line += " ";

			while (second_line.length() < result.find(current->data_) - 2) {
				second_line += "-";
			}
			second_line += "> " + current->data_;

			while (third_line.length() < result.find(current->data_)) {
				third_line += " ";
			}
			third_line += std::to_string(current->gap_);

			current_jump = current->jump_;
		}
		current = current->next_;
	}

	return result + "\n" + second_line + "\n" + third_line;
}

Node* JumpList::findJumpNodeForInsert(const string& s) const {
	Node* current = head_;
	while (current->jump_ != nullptr && current->jump_->data_ < s) {
		current = current->jump_;
	}
	return current;
}

Node* JumpList::findInsertionPoint(Node* segmentStart, const string& s) {
	Node* previous = segmentStart;
	Node* current = segmentStart->next_;

	while (current != nullptr && current->data_ < s) {
		previous = current;
		current = current->next_;
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

void JumpList::splitSegment(Node* jumpNode, int segmentSize) {
	if (segmentSize > MAX_GAP_SIZE) {
		int firstHalf = (segmentSize + 1) / 2;

		Node* temp = jumpNode;
		for (int i = 0; i < firstHalf - 1; i++) {
			temp = temp->next_;
		}

		Node* newJumpNode = temp->next_;
		newJumpNode->jump_ = jumpNode->jump_;
		jumpNode->jump_ = newJumpNode;

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
		splitSegment(jumpNode, segmentSize);
		jumpNode = jumpNode->jump_;
	}
}

bool JumpList::insert(const string& s) {
	if (find(s)) return false;

	Node* newNode = new Node(s, nullptr, nullptr, 0);

	if (head_ == nullptr) {
		head_ = newNode;
		head_->gap_ = 1;
		return true;
	}

	if (s < head_->data_) {
		newNode->next_ = head_;
		newNode->gap_ = head_->gap_;
		newNode->jump_ = head_->jump_;
		head_ = newNode;
	} else {
		Node* segmentStart = findJumpNodeForInsert(s);
		Node* insertPoint = findInsertionPoint(segmentStart, s);

		newNode->next_ = insertPoint->next_;
		insertPoint->next_ = newNode;
	}

	updateSegmentStructure();

	return true;
}

Node* JumpList::findJumpNodeForErase(const string& s) const {
	Node* currentJump = head_;
	while (currentJump->jump_ != nullptr && currentJump->jump_->data_ <= s) {
		if (currentJump->jump_->data_ == s) {
			return currentJump;
		}
		currentJump = currentJump->jump_;
	}
	return currentJump;
}

void JumpList::restructureSegmentAfterErase(Node* currentJump, Node* toDelete) {
	if (toDelete->gap_ == 1 && currentJump->gap_ == 1) {
		currentJump->jump_ = toDelete->jump_;
		currentJump->gap_ = 1;

		Node* previous = currentJump;
		while (previous->next_ != toDelete) {
			previous = previous->next_;
		}
		previous->next_ = toDelete->next_;
	} else {
		int newGap = currentJump->gap_ + toDelete->gap_ - 1;
		currentJump->jump_ = toDelete->jump_;

		Node* previous = currentJump;
		while (previous->next_ != toDelete) {
			previous = previous->next_;
		}
		previous->next_ = toDelete->next_;

		if (newGap > MAX_GAP_SIZE) {
			int firstHalf = (newGap + 1) / 2;
			Node* newJump = currentJump;
			for (int i = 0; i < firstHalf; i++) {
				newJump = newJump->next_;
			}
			newJump->jump_ = currentJump->jump_;
			currentJump->jump_ = newJump;
			newJump->gap_ = newGap - firstHalf;
			currentJump->gap_ = firstHalf;
		} else {
			currentJump->gap_ = newGap;
		}
	}
}

Node* JumpList::findNodeToErase(Node* segmentStart, const string& s) {
	Node* previous = segmentStart;
	Node* current = segmentStart->next_;

	while (current != nullptr && current->data_ != s) {
		if (current->data_ > s) return nullptr;
		previous = current;
		current = current->next_;
	}

	return current;
}

bool JumpList::erase(const string& s) {
	if (head_ == nullptr) return false;

	if (head_->data_ == s) {
		Node* oldHead = head_;
		if (head_->next_ == nullptr) {
			delete head_;
			head_ = nullptr;
		} else {
			head_ = head_->next_;
			if (oldHead->jump_ != head_) {
				head_->jump_ = oldHead->jump_;
				head_->gap_ = oldHead->gap_ - 1;
			}

			if (head_->gap_ == 0) head_->gap_ = 1;
			delete oldHead;
		}
		return true;
	}

	Node* currentJump = findJumpNodeForErase(s);
	if (currentJump->jump_ != nullptr && currentJump->jump_->data_ == s) {
		Node* toDelete = currentJump->jump_;
		restructureSegmentAfterErase(currentJump, toDelete);
		delete toDelete;
		return true;
	}

	Node* nodeToDelete = findNodeToErase(currentJump, s);
	if (nodeToDelete == nullptr) return false;

	Node* previous = currentJump;
	while (previous->next_ != nodeToDelete) {
		previous = previous->next_;
	}

	previous->next_ = nodeToDelete->next_;
	currentJump->gap_--;
	delete nodeToDelete;
	return true;
}