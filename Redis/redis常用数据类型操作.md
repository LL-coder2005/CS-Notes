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
