export default class Queue {
    constructor(size) {
      this.last = undefined
      this.first = undefined
      this.count = 0
      this._ticketSize = size
    }
    skip(element) {
      if(!element) {
        return
      }
      var tmp = this.first
      this.first = {
        element,
        next: undefined,
        prev: tmp
      }
      if(this.count === 0) {
        this.last = this.first
      }
      if(tmp) {
        tmp.next = this.first
      }
      this.count++
    }
    enqueue(element) {
      if(!element) {
        return
      }
      var tmp = this.last
      this.last = {
        element,
        next: tmp,
        prev: undefined
      }
      if(this.count === 0) {
        this.first = this.last
      }
      if(tmp) {
        tmp.prev = this.last
      }
      this.count++
    }
    dequeue() {
      if(this.isEmpty) {
        return
      }
      const item = this.first;
      this.first = item ? item.prev : undefined
      this.count--
      return item.element
    }
    peek() {
      return this.first ? this.first.element : undefined
    }
    remove(element) {
      let head = this.last
      while(head !== undefined) {
        if(head.element === element) {
          if(head.next) {
            head.next.prev = head.prev
          }
          if(head.prev) {
            head.prev.next = head.next
          } else {
            this.last = undefined
          }
          this.count--
          break
        }
        head = head.next
      }
    }
    get length() {
      return this.count;
    }
    get isEmpty() {
      return this.count === 0;
    }
    get ticketSize() {
      return this._ticketSize
    }
}