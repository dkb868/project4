//
// Created by mitrikyle on 3/6/16.
//

#include "DiskList.h"



DiskList::DiskList(const std::string &filename) : head(0), deleted_head(0) , m_size(0){
    // if it exists, get the values there,
    if (m_bf.openExisting(filename)){
        cout << "EXISTING FILE USED! \n";
        // get head from offset 0,
        m_bf.read(head,HEAD_OFFSET);
        // get dleted head from offset 1
        m_bf.read(deleted_head,DELETED_HEAD_OFFSET);
        // get the size from offset 8;
        m_bf.read(m_size, SIZE_OFFSET);
        cout << "SIZE OF EXISTING FILE " << m_size << endl;
    } else {
        cout << "NEW FILE CREATED! \n";
        m_bf.createNew(filename);
        // set the head to point to nothing since list is empty
        // the head can always be found at offset 0 and contains an offset pointer
        m_bf.write(head, HEAD_OFFSET);
        // deleted head can always be found at offset 4 and contains an offset pointer
        m_bf.write(deleted_head, DELETED_HEAD_OFFSET);
        // size can always be found at offset 8 and contains an int for the size
        m_bf.write(m_size, SIZE_OFFSET);
        // m size will only be incremented, and will be used to figure out
        // wher to put newly inserted elements
    }
}

bool DiskList::push_front(const char data) {
    // if the list was empty, just make head equal to this value
    // if the list is not empty then put head in another place, then put this value in head
    // create new node,pointing to head
    DiskNode p(data,head);

    // CHECK TO SEE IF WE HAVE ANY DELETED NODES
    if (deleted_head != 0){
        // if we do then put the value on one of thsoe!
        // store the deleted head in temp
        DiskNode temp;
        m_bf.read(temp, deleted_head);
        // write p to deleted head's offset
        p.offset = deleted_head;
        m_bf.write(p,deleted_head);

        // set head to p
        head = p.offset;
        // the head can always be found at offset 0 and contains an offset pointer
        m_bf.write(head, HEAD_OFFSET);
        // poitn deleted head to its next value
        deleted_head = temp.next;
        // deleted head can always be found at offset 1 and contains an offset pointer
        m_bf.write(deleted_head,DELETED_HEAD_OFFSET);
        return true;
    }
    // OTHERWISE JUST EXPAND THE FILE SIZE

    // write the new node to the file at the current latest offset
    // which is the size of the list (or the size of 1 item was added) multiplied by the size of a disk node
    BinaryFile::Offset offset = sizeof(DiskNode) * (m_size+1);
    // set the nodes offset to the place where it was palced
    p.offset = offset;
    // add to the latest offset
    // currently m_size is only incremented
    m_bf.write(p, offset);
    m_size++;
    m_bf.write(m_size, SIZE_OFFSET);
    // set head to p
    head = p.offset;
    // the head can always be found at offset 0 and contains an offset pointer
    m_bf.write(head, HEAD_OFFSET);
    return true;
}

void DiskList::printAll() {
    cout << "SIZE: " << m_bf.fileLength();
    cout << "SIZE OF NDOE : " << sizeof(DiskNode) << endl;
    cout << "SIZE OF INT: " << sizeof(int) << endl;
    // iterato?
    BinaryFile::Offset p;
    p = head;
    DiskNode temp(0,0);
    while (true){
        m_bf.read(temp, p);
        cout << temp.value << endl;
        // if we are at the last element, berak
        if (temp.next== 0)
            break;
        // otherwise assign p to the next value
        // assign p to the next value
        p = temp.next;
    }
}

// push a deleted node to the front of the deleted list
bool DiskList::push_front_deleted(BinaryFile::Offset offset) {
    // if the list was empty, just make head equal to this value
    // if the list is not empty then put head in another place, then put this value in head
    // create new node,pointing to head

    DiskNode p;
    // get the node from the file
    m_bf.read(p, offset);
    // point that thing to head
    p.next = deleted_head;
    // point head to p;
    deleted_head = p.offset;
    // deleted head can always be found at offset 1 and contains an offset pointer
    m_bf.write(deleted_head,DELETED_HEAD_OFFSET);
    // save our data

    m_bf.write(p, p.offset);
    return true;
}



bool DiskList::remove(const char data) {
    // if the lsit is empty then return
    if (head == 0)
        return false;
    // if we want ot delete first
    // point head to the second node
    BinaryFile::Offset p = head;
    DiskNode temp(0,0);
    // temp gets the value of head
    m_bf.read(temp,p);
    if (temp.value == data) {
        // head = p->next
        head = temp.next;
        // the head can always be found at offset 0 and contains an offset pointer
        m_bf.write(head, HEAD_OFFSET);
        push_front_deleted(temp.offset);
        return true;
    }
    DiskNode q(0,0);

    while (true){
        // if we get to the end, rbeak the loop
        if(temp.next == 0)
            break;

        m_bf.read(q,temp.next);
        if (q.value == data)
            break; // temp points to node above

        // else continue iterating
        m_bf.read(temp, temp.next);
    }
    // if we actually found it
    if (q.value == data){
        temp.next = q.next;
        // delete q
        push_front_deleted(q.offset);
        m_bf.write(temp, temp.offset);
        return true;
    }
    return false;
}
