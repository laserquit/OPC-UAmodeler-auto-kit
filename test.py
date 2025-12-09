import asyncio
from asyncua import Client
from asyncua.common.subscription import DataChangeNotificationHandler

# 配置连接参数（需与服务端匹配）
SERVER_URL = "opc.tcp://localhost:48400/opcua_server/"
NAMESPACE_URI = "http://examples.io"

class DataChangeHandler(DataChangeNotificationHandler):
    """
    自定义数据变化处理器
    """
    def datachange_notification(self, node, val, data):
        print(f"\n[数据变化通知]")
        print(f"节点: {node}")
        print(f"新值: {val}")
        print(f"时间戳: {data.monitored_item.Value.SourceTimestamp}")
        print(f"状态码: {data.monitored_item.Value.StatusCode}")

async def main():
    print(f"正在连接服务器: {SERVER_URL}")
    
    async with Client(url=SERVER_URL) as client:
        try:
            # 获取命名空间索引
            nsidx = await client.get_namespace_index(NAMESPACE_URI)
            print(f"命名空间 '{NAMESPACE_URI}' 的索引号: {nsidx}")

            # 构建目标节点路径
            var_node = await client.nodes.root.get_child([
                "0:Objects", 
                f"{nsidx}:MyObject", 
                f"{nsidx}:MyVariable"
            ])
            
            # 读取初始值
            init_val = await var_node.read_value()
            print(f"\n当前变量值: {init_val}")

            # 创建订阅（ms发布间隔）
            handler = DataChangeHandler()
            subscription = await client.create_subscription(2000, handler)
            
            # 订阅数据变化
            handle = await subscription.subscribe_data_change(var_node)
            print(f"\n已订阅变量变化 (句柄: {handle})")
            print("等待数据变化通知... (按Ctrl+C停止)")

            # 保持连接
            while True:
                await asyncio.sleep(1)

        except asyncio.CancelledError:
            print("\n用户中断，正在清理...")
        finally:
            if 'subscription' in locals():
                await subscription.delete()
                print("订阅已删除")
            print("客户端已断开")

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("程序终止")