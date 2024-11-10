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
		while (current -> jump_ != nullptr) {
			Node* tmp = current -> next_;
			string space;
			while (tmp != current -> jump_) {
				space += "----";
				tmp = tmp -> next_;
			}
			space += ">";
			result += " " + space + " " + current -> jump_ -> data_;
			current = current -> jump_;
		}
	}
	result += "\n";

	current = head_;
	if (current != nullptr) {
		result += std::to_string(current -> gap_);
		while (current -> jump_ != nullptr) {
			Node* tmp = current -> next_;
			string space;
			while (tmp != current -> jump_) {
				space += "    ";
				tmp = tmp -> next_;
			}
			space += " ";
			result += space + std::to_string(current -> jump_ -> gap_);
			current = current -> jump_;
		}
	}

	return result;
}

bool JumpList::insert(const string& s) {
	if (find(s)) return false;

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
	}
	else {
		newNode -> next_ = current;
		previous -> next_ = newNode;
	}

	Node* jumpNode = head_;
	while (jumpNode != nullptr) {
		int segmentSize = 1;
		Node* temp = jumpNode -> next_;
		while (temp != jumpNode -> jump_) {
			if (temp == nullptr) break;
			segmentSize++;
			temp = temp -> next_;
		}

		if (segmentSize > MAX_GAP_SIZE) {
			int firstHalf = (segmentSize + 1) / 2;
			temp = jumpNode;
			for (int i=0; i < firstHalf; i++) {
				temp = temp -> next_;
			}

			temp -> jump_ = jumpNode -> jump_;
			jumpNode -> jump_ = temp;

			temp -> gap_ = segmentSize - firstHalf;
			jumpNode -> gap_ = firstHalf;
		}
		else {
			jumpNode -> gap_ = segmentSize;
		}

		jumpNode = jumpNode -> jump_;
	}

	return true;
}

bool JumpList::erase(const string& s) {
    if (head_ == nullptr) return false;

    // Using find() pattern to locate the node
    Node* jumpNode = head_;
    Node* prevJump = nullptr;

    // Fast lane traversal
    while (jumpNode->jump_ != nullptr && jumpNode->jump_->data_ < s) {
        prevJump = jumpNode;
        jumpNode = jumpNode->jump_;
    }

    // Slow lane traversal
    Node* prev = nullptr;
    Node* current = jumpNode;
    while (current != nullptr && current->data_ != s) {
        if (current->data_ > s) return false;
        prev = current;
        current = current->next_;
    }

    if (current == nullptr) return false;

    // Found node to delete
    bool isJumpNode = (current->jump_ != nullptr);

    // Case 1: Removing head
    if (prev == nullptr) {
        head_ = current->next_;

        if (head_ != nullptr) {
            // Transfer jump pointer if head was jump node
            if (isJumpNode) {
                head_->jump_ = current->jump_;

                // Recount gap for new head segment
                Node* temp = head_;
                int count = 1;
                while (temp->next_ != nullptr && temp->next_ != current->jump_) {
                    count++;
                    temp = temp->next_;
                }
                head_->gap_ = count;
            } else {
                head_->gap_ = current->gap_ - 1;
            }
        }

        delete current;
        return true;
    }

    // Case 2: Regular node removal
    prev->next_ = current->next_;

    // Handle jump node removal
    if (isJumpNode) {
        // Find previous jump node
        Node* prevJumpNode = head_;
        while (prevJumpNode->jump_ != current) {
            prevJumpNode = prevJumpNode->jump_;
        }

        Node* nextJump = current->jump_;
        prevJumpNode->jump_ = nextJump;

        // Count nodes in merged segment
        int mergedCount = 0;
        Node* temp = prevJumpNode;
        while (temp != nullptr && temp != nextJump) {
            mergedCount++;
            temp = temp->next_;
        }

        // Handle potential split
        if (mergedCount > MAX_GAP_SIZE) {
            int firstHalf = (mergedCount + 1) / 2;

            // Find split point
            temp = prevJumpNode;
            for (int i = 1; i < firstHalf; i++) {
                temp = temp->next_;
            }

            // Create new jump node
            Node* newJump = temp->next_;
            prevJumpNode->jump_ = newJump;
            newJump->jump_ = nextJump;

            // Update gap sizes
            prevJumpNode->gap_ = firstHalf;
            newJump->gap_ = mergedCount - firstHalf;
        } else {
            prevJumpNode->gap_ = mergedCount;
        }
    } else {
        // Regular node removal - update containing segment's gap
        Node* containingJump = head_;
        while (containingJump != nullptr) {
            Node* nextJump = containingJump->jump_;

            // Check if current node is in this segment
            Node* temp = containingJump;
            bool inSegment = false;
            while (temp != nextJump) {
                if (temp == current) {
                    inSegment = true;
                    break;
                }
                if (temp == nullptr) break;
                temp = temp->next_;
            }

            if (inSegment) {
                containingJump->gap_--;
                break;
            }

            containingJump = containingJump->jump_;
        }
    }

	// Update all remaining gaps after modification
	Node* temp = head_;
	while (temp != nullptr && temp->jump_ != nullptr) {
		int count = 1;
		Node* counter = temp;
		// Add nullptr check before accessing next_
		while (counter != nullptr && counter->next_ != nullptr && counter->next_ != temp->jump_) {
			count++;
			counter = counter->next_;
		}
		temp->gap_ = count;
		temp = temp->jump_;
	}

	// Handle last segment if it exists
	if (temp != nullptr) {
		int lastCount = 1;
		Node* counter = temp;
		// Add nullptr check before accessing next_
		while (counter != nullptr && counter->next_ != nullptr) {
			lastCount++;
			counter = counter->next_;
		}
		temp->gap_ = lastCount;
	}

    delete current;
    return true;
}