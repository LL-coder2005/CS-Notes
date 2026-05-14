### redis中对key的操作

`select 编号`:切换数据库

`dbsize`: 查看数据库中键值对个数

`keys *`: 查看数据库中键值

`flushdb`: 清空当前数据库中键值对

`set [key] [value]`： 设置键值对

`type [key]`: 查看键值类型

`exists [key]`: 是否存在

`expire [key] [time]`: 设置键值存在时间

`ttl [key]`: 剩余时间

`strlen [key]`: 对应值的长度

`append [key]`: 在对应值的后面追加内容

`setnx [key] [value]`: 如果不存在，则设置

`incr [key num]`: 使键对应的数字值自增

`incrby [key] [num]`: 自增num

`decr`和`decrby`正好相反，自减操作

值得注意的是，因为redis是单线程的，所以`incrby`自增操作是原子性的。而编程中使用多线程自增则不是原子性的，需要配合锁使用。