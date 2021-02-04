using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public class CUtil
{
    public delegate void SendMsgDele(int msgID,IntPtr buf,int len);

    public static SendMsgDele gSendMsgDele = null;

    public static string BootStrap()
    {
        return "BootStrap";
    }

    public static int Add(int a, int b)
    {
        return a + b;
    }

    public static void SetSendMsgDele(SendMsgDele dele)
    {
        gSendMsgDele = dele;

        //protobuf msg
        string str = "123"; 
        byte[] datas = System.Text.Encoding.Default.GetBytes(str);

        //byte[] datas = new byte[] { 1, 2, 3 };

        //方法一
        IntPtr buff = Marshal.AllocHGlobal(datas.Length);
        Marshal.Copy(datas, 0, buff, datas.Length);
        gSendMsgDele(1, buff, datas.Length);
        Marshal.FreeHGlobal(buff);


        ////方法二(日志输出不对,可能编码问题)
        //GCHandle gch = GCHandle.Alloc(datas);
        //IntPtr p = GCHandle.ToIntPtr(gch);
        //gSendMsgDele(1, p, datas.Length);
        //gch.Free();
    } 
}


