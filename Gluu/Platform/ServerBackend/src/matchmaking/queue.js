export default class Queue {
    constructor(size) {
      this.first = undefined
      this.last = undefined
      this.count = 0
      this._matchSize = size
    }
    enqueue(element) {
      var tmp = this.last
      this.last = {
        element,
        next: undefined,
        prev: tmp
      };
      if(this.count === 0) {
        this.first = this.last
      }
      if(tmp) {
        tmp.next = this.last
      }
      this.count++
    }
    dequeue() {
      const item = this.last;
      this.last = item ? item.prev : undefined
      this.count--
      return item.element
    }
    peek() {
      return this.last.element
    }
    remove(element) {
      let head = this.first
      while(head !== undefined) {
        if(head.element === element) {
          if(head.next) {
            head.next.prev = head.prev
          }
          if(head.prev) {
            head.prev.next = head.next
          } else {
            this.first = undefined
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
    get matchSize() {
        return this._matchSize
    }
}