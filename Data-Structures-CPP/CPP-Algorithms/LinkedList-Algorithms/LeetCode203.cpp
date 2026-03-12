/** 
 * 力扣题目203
 * 移除链表中的元素，因为链表中删除某个节点需要从上一个节点操作
 * 为了保证逻辑一致性，可以引入哨兵节点。
*/


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
        //设置哨兵节点，这样后续处理的逻辑就一致了
        ListNode* dummy_head = new ListNode(0,head);
        ListNode* cur = dummy_head;
        while(cur->next){
            //如果当前节点的下一个节点值为目标值，则进行删除操作
            if(cur->next->val==val){
                ListNode* temp = cur->next;
                cur->next = temp->next;
                delete(temp);
            }
            //否则移到下一个节点
            else{
                cur = cur->next;
            }
        }
        //因为设置了哨兵节点，所以dummy_head->next才是真正的头节点
        return dummy_head->next;
    }
};