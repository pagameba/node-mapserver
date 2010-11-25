/**
 * Class: Cache
 * A simple, hopefully efficient, in-memory caching system.  The basic concept
 * of the cache is to use a modified Least Recently Used (LRU) displacement 
 * algorithm that also tracks the number of cache hits for each cached object.
 * The cache maintains several LRU queues.  An object that gets multiple cache
 * hits gets promoted periodically to the next higher queue.  As objects get
 * stale in a higher queue and are displaced by new items, they get demoted to
 * lower queues.  Queues are allocated on integral values of ln(<hit count>) 
 * which means that there is a queue for 1 hit, a queue for 2-10, a queue for
 * 11-100 and so on.
 */

var Cache = function(max) {
  // hash for all objects so we can find them relatively quickly
  var cache = {};
  
  /**
   * Class: Queue
   * manage a linked list of cache objects to a maximum size
   */
  var Queue = function(max) {
    var n = 0,
        that = this,
        last = null;
    that.next = null;
    /**
     * Method: push
     * push an object at the head of the queue
     * 
     * Parameters:
     * o - the object ot push
     *
     * Returns: an object displaced from the bottom of the queue if the queue
     * is full or null.
     */
    this.push = function(o) {
      if (!o) return;
      if (that.next) {
        o.next = that.next;
        that.next = o;
      } else {
        o.next = null;
        last = o;
      }
      o.prev = that;
      o.q = this;
      n++;
      if (n>max) {
        return this.pop();
      }
    };
    /**
     * Method: pop
     * pull an object off the bottom of the queue and return it
     */
    this.pop = function() {
      if (last) {
        n--;
        var o = last;
        last.prev.next = null;
        last = last.prev;
        return o;
      }
    };
    /**
     * Method: remove
     * remove an object from this queue if it is owned by this queue and
     * return it, or return null if the object is not owned by this queue.
     *
     * Parameters:
     * o - the object to remove
     */
    this.remove = function(o) {
      if (!o || !o.queue == this) return null;
      o.prev.next = o.next;
      if (o.next) {
        o.next.prev = o.prev;
      }
      o.queue = null;
      return o;
    };
    /**
     * Method: promote
     * promote an object to the top of the queue
     *
     * Parameters:
     * o - the object to promote
     */
    this.promote = function(o) {
      this.push(this.remove(o));
    };
  };
  
  /**
   * Method: get
   * return an object from the cache and refresh it, or null if not found
   *
   * Parameters:
   * key - the key by which the data is identified
   */
  this.get = function(key) {
    var o = cache[key],
        q,
        qn,
        popped;
    if (o) {
      // track the cache hit
      o['time'] = Date();
      o.count += 1;
      // check the queue based on power of 10 on hit count
      qn = Math.ceil(Math.log(o.count)/Math.LN10);
      q = queues[qn];
      if (!q) {
        var ql = Math.floor(max / Math.pow(2,qn+1));
        if (ql) {
          queues[qn] = q = new Queue(ql);
        } else {
          qn --;
          q = queues[qn];
        }
      }
      if (o.queue != q) {
        if (o.queue) {
          o.queue.remove(o);
        }
        q.push(o);
      } else {
        // promote to top of current queue
        q.promote(o);
      }
    }
    return o;
  };
  
  /**
   * Method: set
   * put some data in the Cache.  If the data is already in the cache,
   * update it.
   *
   * Parameters:
   * key - the key by which the data is identified
   * data - arbitrary data to store in the cache.
   */
  this.set = function(key, data) {
    var o = this.get(key),
        popped;
    if (!o) {
      o = {
        'time': Date(),
        data: data,
        count: 1
      };
      cache[key] = o;
      // discard things popped from the 0th queue
      queues[0].push(o);
    } else {
      o.data = data;
    }
    return o;
  };
  
  /**
   * Method: cascade
   * cascade an object being displaced from a queue onto another queue,
   * recursing to handle an object being displaced as a result of this.
   *
   * Parameters:
   * o - the object being cascaded
   * qn - the queue number that the object is being cascaded onto
   */
  this.cascade = function(o, qn) {
    var q = queues[qn];
    if (o && q) {
      o.count = Math.pow(10,qn) - 1;
      this.cascade(q.push(o, qn-1));
    }
  };
  
  var queues = [new Queue(Math.floor(max / 2))];
  
};

exports.Cache = Cache;