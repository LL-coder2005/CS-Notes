
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
 
class Solution {
public:
    ListNode* removeElements(ListNode* head, int val) {
        ListNode* dummy_head = new ListNode(0,head);
        ListNode* cur = dummy_head;
        while(cur->next){
            if(cur->next->val==val){
                ListNode* temp = cur->next;
                cur->next = temp->next;
                delete(temp);
            }
            else{
                cur = cur->next;
            }
        }
        
        return dummy_head->next;
    }
};