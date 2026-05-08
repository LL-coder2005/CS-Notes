# 应用层 

### DHCP动态主机配置协议  

DHCP为客户端服务器模式  

客户端端口号：67  
服务器端口号：68  
运输层使用的协议：**UDP**  

计算机想要连接互联网通常需要设置以下参数：  
- ip地址：一台电脑的唯一标识
- 子网掩码：通过与ip地址进行位与运算得到网络号
- 默认网关：通过这个地址连接外网，通常是路由器的接口地址
- 本地DNS服务器地址：进行域名解析  

如果采用手动配置不仅工作量大，而且容易出错，DHCP因而被设计用于为主机提供自动化参数分配。

当一台新主机连接到网络时：
1. 发现阶段：主机源地址为 **0.0.0.0**，向广播地址 255.255.255.255 发送DHCPDISCOVER，请求可用地址。
2. 提供阶段：一个或多个DHCP服务器返回DHCPOFFER，提供可用IP地址及其它配置参数。
3. 选择阶段：主机发送DHCPREQUEST广播，选择某一服务器的地址方案。
4. 确认阶段：被选定的DHCP服务器发送DHCPACK确认，主机即可以使用该地址。

只有在上述四个阶段成功完成后，主机才能正式绑定并使用分配到的IP地址。

等到到达租赁期一半的时间时，主机会向对应的那台DHCP服务器发送续租请求
此时有三种可能性  
1. DHCP服务器确认同意请求，则重新计时  
2. DHCP服务器确认拒绝请求，则主机立即停止使用此ip地址，重新请求配置
3. DHCP服务器未响应，主机在租赁期的0.875时再次请求续租  
时间结束则重新请求  

中继代理：DHCP服务器与主机不一定要在同一个局域网内，可以将路由器设为中继代理
会将主机的广播转为路由器对服务器的单播。

---  

## DNS域名解析

### 1. DNS基础与分层结构（必背）

- DNS命名空间是“树形结构”，从根（"."）开始向下分层：根 -> 顶级域（TLD） -> 二级域 -> 子域。树形结构便于委派管理和分布式解析，避免名称冲突。
- 主要服务器角色：
  - 根服务器（Root Server）：全球 13 个逻辑节点（A-M），负责指向 TLD 服务器地址；本地DNS通常先向根请求.
  - 顶级域服务器（TLD Server）：管理 `.com`、`.cn`、`.net` 等顶级域，是向权威名称服务器转发的关键.
  - 权威域名服务器（Authoritative DNS）：存储域名实际资源记录（A、MX 等），对该域有权威回答.

- 推荐背诵：根 -> .com等 -> example.com -> www.example.com.

### 2. DNS查询流程（递归 + 迭代）

- 客户端（本地宿主或应用）首先询问本地DNS解析器。
- 本地DNS向递归解析器（通常ISP或企业DNS）发起递归查询（RD=1），递归解析器负责迭代查询过程：
  1. 向根服务器请求 `www.example.com` 的信息（返回 `.com` TLD 列表）.
  2. 向 `.com` TLD 服务器请求（返回 `example.com` 的权威NS）.
  3. 向 `example.com` 权威服务器请求（返回 `www.example.com` 的A/AAAA记录）.
  4. 返回给本地DNS并缓存; 本地DNS返回给客户端.
- 迭代查询：客户端问某服务器（通常本地DNS）服务器每次只请求下一层，如果返回域名服务器地址，由客户端或本地DNS继续向下查询.

### 3. 端口与传输层

- DNS标准端口：53
- 传输层协议：UDP（首选，查询/响应、性能高）
- TCP：当UDP响应超过512字节(或EDNS0扩展为更大)、区域传送（AXFR/IXFR）、DNSSEC（签名）等场景使用

### 4. 常见记录类型（必须会）

- A：IPv4地址
- AAAA：IPv6地址
- CNAME：别名（Canonical Name，指向另一域名），注意：不与其他记录共存于同一标签
- NS：指定委派域的权威服务器
- MX：邮件交换记录，带优先级，邮箱传送使用
- PTR：反向解析，IP -> 域名（一般在 `in-addr.arpa` / `ip6.arpa`）
- SOA：起始授权记录，包含域名同步、刷新、重试、过期、TTL等
- TXT：任意文本（如SPF、DKIM等）

### 5. DNS报文结构与重要标志

- DNS报文分为两部分：头部（Header）和可变长度的四个区域。

1. Header（固定12字节）：
   - ID（16位）: 查询/响应匹配号（客户端生成，响应返回相同ID）
   - 标志位（16位）：
     - QR（1位）：0=查询，1=响应
     - Opcode（4位）：0=标准查询（QUERY）、1=反向查询（IQUERY）等
     - AA（1位）：权威回答（只在响应时有效）
     - TC（1位）：截断（UDP返回报文长度大于限制需TCP重试）
     - RD（1位）：递归请求（客户端要求DNS服务器递归查询）
     - RA（1位）：递归可用（服务器支持递归）
     - Z（3位）：保留，通常为0；AD、CD 等在扩展场景中使用
     - RCODE（4位）：返回码（0=NOERROR、2=SERVFAIL、3=NXDOMAIN、5=REFUSED等）
   - 计数字段（各16位）：Qdcount、Ancount、Nscount、Arcount，分别表示 Question、Answer、Authority、Additional 的记录条数。

2. Question（查询区）：包含域名、类型（QTYPE）、类（QCLASS）

3. Answer（回答区）：包含与查询匹配的资源记录，列出IP、别名等

4. Authority（权威区）：返回域名的权威名称服务器信息（如NS记录）

5. Additional（附加区）：补充额外信息（如A记录、AAAA记录），加速解析

- 常见记录：
  - A/AAAA：地址记录
  - CNAME：别名
  - NS：名称服务器
  - MX：邮件交换
  - SOA：起始授权
  - TXT、SRV等

- 重点：
  - `ID`用来绑定请求与响应
  - `RD=1` 表示请求递归解析； `RA=1` 表示服务器支持递归
  - `TC=1`说明UDP能容纳的数据不足；客户端主机可重试TCP
  - `RCODE=3`说明域名不存在（NXDOMAIN）


### 6. 缓存与TTL（必须掌握）

- TTL（秒）表示资源记录的有效时间，本地/ISP服务器缓存后依TTL过期
- 有利：减少查询延迟、减轻上级服务器压力，提升性能
- 弊端：旧数据停留、变更传播延迟、缓存中毒影响范围更大
- TTL为300秒时：记录缓存300秒后失效，下次请求再从权威DNS重新查询得最新值；并非“立即”更新，需要客户端发起查询.

### 7. 安全扩展（校招/考研加分）

- DNS缓存投毒：攻击者伪造DNS响应，利用ID、端口、交易ID等猜测并注入错误记录。在UDP情况下易发生，尤其非随机源端口.
- DNSSEC：通过公钥签名验证DNS数据完整性与来源，防篡改
  - DNSKEY：域的公钥（包括KSK、ZSK）
  - RRSIG：资源记录签名，绑定DNS数据
  - DS：父域到子域的信任链摘要
  - 验证链：根签名 -> TLD -> 具体域 -> 记录

- DoT（DNS over TLS 853）/DoH（DNS over HTTPS 443）：将DNS查询加密，避免被监听/篡改，隐私更好，绕过DNS污染

### 8. 高可用与运维

- 多权威DNS推荐：冗余、跨站点部署、避免单点故障、负载均衡、地理就近

### 9. 典型流程示例（记忆重点）

1. 浏览器 -> /etc/resolv.conf（或系统DNS设置）
2. 本地DNS递归器（发RD=1）-> 根服务器
3. 根 -> TLD
4. TLD -> 权威
5. 权威->返回A/AAAA
6. 在本地缓存并返回给客户端

> 补充：`dig +trace`是迭代跟踪，从根开始到权威；`dig @1.1.1.1`强制指定上游服务器；`dig -x x.x.x.x`是反向解析。

---

## Web与HTTP 协议  

### URL  
统一资源定位符（Uniform Resource Locator），通常格式为 `协议://主机名:端口/路径`。

## HTTP  
- 端口号：80  
- 运输层协议：TCP  

## HTTP模型

1. 客户端向URL发出请求。
2. 建立TCP连接（通常基于三次握手）。
3. 客户端发送HTTP请求报文。
4. 服务端处理请求并返回HTTP响应报文。
5. 连接关闭或复用（取决于版本与连接头）。

在HTTP/1.0中，默认使用短连接：每次请求/响应之后断开连接。对于包含多个资源的网页，这一方式会导致大量连接开销。

在HTTP/1.1中，引入了持久连接（`Connection: keep-alive`），可以在同一TCP连接内发送多个请求，减少握手延迟和资源消耗。

HTTP/1.x 也有以下优化机制：
- 并行连接：客户端可以同时建立多个TCP连接并行获取多个资源。
- 请求管线化：在同一连接上连续发送多个请求，可减少等待时间，但受队首阻塞（Head-of-line blocking）限制。

HTTP 的另一重要特性是无状态：每个请求独立处理，不保留上次请求的状态。会话机制通过应用层实现，例如Cookie、Session ID等。

Cookie：服务器首次响应时通过 `Set-Cookie` 在客户端设置会话标识，客户端在后续请求中携带该Cookie，服务器据此识别用户状态。这种机制为HTTP提供了“状态关联”的能力，同时需配合安全策略（`HttpOnly`, `Secure`, `SameSite`）使用。

### HTTP版本演进  
- HTTP 1.1  
    实现了HTTP的连接，使得浏览器请求资源时不需要重复建立连接  
    缺点：可能会出现队首堵塞，因为所有的资源请问按顺序执行，当第一个资源特别大时可能会导致后面的资源没办法按时加载。即使使用并行的技术，维护大量的TCP连接同样消耗大量资源  
- HTTP 2  
    1. 多路复用：通过此技术解决了队首堵塞的问题，通过一个TCP连接即可处理。由于TCP的拥塞控制，多个TCP总是倾向于平分带宽，因此使用一个TCP连接不仅能让其更好的发挥作用，还能防止相互争抢带宽。  

      成帧和交错传输机制：这是实现多路复用的核心，把一个资源分成多个帧，可以先发送A的第一个帧，B的第一个帧....，实现多个资源通过一个TCP连接。  
    2. 资源的主动推送：服务器可以主动向浏览器推送其可能需要的资源，当浏览器解析HTML文件时，所需的资源可能已经拥有。  
    3. 请求优先级：服务器可以设置不同的优先级。优先级高的资源将先被推送  

### Web 缓存器

Web 缓存服务器用于缓存来自客户端的资源请求，以减少传输延迟并降低源服务器负载。

当浏览器请求资源时，缓存服务器首先检查本地缓存是否命中。若命中，则直接返回缓存内容；若未命中，则向原始服务器请求资源，返回给客户端的同时在缓存中保存一份。

该机制可显著提高多个用户请求同一资源时的响应速度，并减少带宽和服务器资源消耗。

---

### HTTP 报文结构

- 请求报文：请求行 + 报头字段 + 空行 + 可选消息体
  - 请求行：方法 URL 版本（例如 `GET /index.html HTTP/1.1`）
  - 常见请求方法：`GET`、`POST`、`PUT`、`DELETE`、`HEAD`、`OPTIONS`、`PATCH`、`TRACE`、`CONNECT`
  - 常见请求头：`Host`、`User-Agent`、`Accept`、`Accept-Encoding`、`Accept-Language`、`Connection`、`Content-Type`、`Content-Length`、`Cookie`、`Referer`、`Authorization`、`Origin`

- 响应报文：状态行 + 报头字段 + 空行 + 可选消息体
  - 状态行：版本 状态码 原因短语（例如 `HTTP/1.1 200 OK`）
  - 常见响应头：`Date`、`Server`、`Content-Type`、`Content-Length`、`Set-Cookie`、`Cache-Control`、`Expires`、`Last-Modified`、`ETag`、`Location`、`Access-Control-Allow-Origin`、`Keep-Alive`

- 报文头类型
  - 通用头（General）：对请求/响应都适用，例如 `Cache-Control`、`Connection`、`Date`、`Via`。
  - 请求头（Request）：例如 `Accept`、`Host`、`Authorization`、`Cookie`。
  - 响应头（Response）：例如 `Location`、`Server`、`Set-Cookie`。
  - 实体头（Entity）：包含资源表示信息，例如 `Content-Type`、`Content-Length`、`Content-Encoding`、`Content-Language`。

### HTTP 方法与语义（必会）

- GET：读取资源，安全、幂等，无请求体（通常）；缓存友好。不能用于修改服务端状态。
- HEAD：与 GET 相同但无消息体，用于获取头信息、检测资源是否变化。
- POST：创建/提交数据，不幂等。常用于表单提交和API写操作。
- PUT：更新或创建资源，幂等（同样请求多次结果一致）。
- DELETE：删除资源，幂等（逻辑上）。
- PATCH：部分更新资源，非幂等。
- OPTIONS：查询服务器或目标资源支持的方法和功能（跨域预检）。
- CONNECT：创建到代理的隧道（HTTPS透传）。
- TRACE：追踪请求路径（调试用途），应谨慎使用。

### HTTP 状态码（核心）

- 1xx（信息性）：`100 Continue`、`101 Switching Protocols`。
- 2xx（成功）：`200 OK`、`201 Created`、`202 Accepted`、`204 No Content`。
- 3xx（重定向）：`301 Moved Permanently`、`302 Found`、`303 See Other`、`307 Temporary Redirect`、`308 Permanent Redirect`。
- 4xx（客户端错误）：`400 Bad Request`、`401 Unauthorized`、`403 Forbidden`、`404 Not Found`、`405 Method Not Allowed`、`412 Precondition Failed`、`429 Too Many Requests`。
- 5xx（服务器错误）：`500 Internal Server Error`、`502 Bad Gateway`、`503 Service Unavailable`、`504 Gateway Timeout`。

### 连接管理与性能优化

- HTTP/1.0 默认短连接；HTTP/1.1 默认持久连接（Connection: keep-alive）。
- `Connection: close` 关闭持久连接。
- `Keep-Alive` 可指定超时与最大请求数（服务器可选支持）。
- HTTP/1.1 管线化：在同一TCP连接中可以发送多个请求无需等待响应，但被队首堵塞（Head-of-line blocking）限制。
- HTTP/2 多路复用：单一TCP连接承载多个流，消除管线化缺陷，避免队首堵塞，提升性能。
- HTTP/3（QUIC）基于UDP，不依赖TCP，解决TCP握手与队首堵塞问题。




### REST 和 API 设计要点

- REST原则：资源URI表示名词；使用HTTP方法表达操作；无状态；可缓存；统一接口。
- 版本控制：URI版本(`/v1/users`)或请求头版本(`Accept: application/vnd.example.v1+json`)。
- 常用状态码：`200`、`201`、`204`、`400`、`401`、`403`、`404`、`409`、`422`、`500`。


### 典型 HTTP 请求流程（补充）

1. DNS解析域名到IP。
2. 建立TCP三次握手（SYN、SYN-ACK、ACK）。
3. 如果HTTPS，TLS握手完成。
4. 发送HTTP请求报文。
5. 服务器处理并返回HTTP响应报文。
6. 浏览器解析HTML、CSS、JS、图片，发起额外资源请求。
7. 持久连接下可复用TCP连接；非持久则关闭。
8. 浏览器根据Cache-Control等做缓存。

### 常见面试考点

- 分别解释 HTTP/1.0、HTTP/1.1、HTTP/2、HTTP/3 优势。
- TCP 连接与 HTTP 请求关系，为什么HTTP/2允许多路复用。
- 解释 HTTP 状态码 `304` 与 `200` 的区别。
- 说明 `Cache-Control: no-cache` vs `no-store`。
- 说明 `CORS` 的预检过程与 `Access-Control-Allow-Origin`。
- 说明 HTTPS 的证书链验证与握手流程。

---

