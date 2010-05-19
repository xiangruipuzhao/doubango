/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.39
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class SipStack : SafeObject {
  private HandleRef swigCPtr;

  internal SipStack(IntPtr cPtr, bool cMemoryOwn) : base(tinyWRAPPINVOKE.SipStackUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(SipStack obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~SipStack() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        tinyWRAPPINVOKE.delete_SipStack(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public SipStack(SipCallback callback, string realm_uri, string impi_uri, string impu_uri) : this(tinyWRAPPINVOKE.new_SipStack(SipCallback.getCPtr(callback), realm_uri, impi_uri, impu_uri), true) {
  }

  public bool start() {
    bool ret = tinyWRAPPINVOKE.SipStack_start(swigCPtr);
    return ret;
  }

  public bool setRealm(string realm_uri) {
    bool ret = tinyWRAPPINVOKE.SipStack_setRealm(swigCPtr, realm_uri);
    return ret;
  }

  public bool setIMPI(string impi) {
    bool ret = tinyWRAPPINVOKE.SipStack_setIMPI(swigCPtr, impi);
    return ret;
  }

  public bool setIMPU(string impu_uri) {
    bool ret = tinyWRAPPINVOKE.SipStack_setIMPU(swigCPtr, impu_uri);
    return ret;
  }

  public bool setPassword(string password) {
    bool ret = tinyWRAPPINVOKE.SipStack_setPassword(swigCPtr, password);
    return ret;
  }

  public bool setProxyCSCF(string fqdn, uint port, string transport, string ipversion) {
    bool ret = tinyWRAPPINVOKE.SipStack_setProxyCSCF(swigCPtr, fqdn, port, transport, ipversion);
    return ret;
  }

  public bool setLocalIP(string ip) {
    bool ret = tinyWRAPPINVOKE.SipStack_setLocalIP(swigCPtr, ip);
    return ret;
  }

  public bool setLocalPort(uint port) {
    bool ret = tinyWRAPPINVOKE.SipStack_setLocalPort(swigCPtr, port);
    return ret;
  }

  public bool addHeader(string name, string value) {
    bool ret = tinyWRAPPINVOKE.SipStack_addHeader(swigCPtr, name, value);
    return ret;
  }

  public bool removeHeader(string name) {
    bool ret = tinyWRAPPINVOKE.SipStack_removeHeader(swigCPtr, name);
    return ret;
  }

  public bool addDnsServer(string ip) {
    bool ret = tinyWRAPPINVOKE.SipStack_addDnsServer(swigCPtr, ip);
    return ret;
  }

  public bool isValid() {
    bool ret = tinyWRAPPINVOKE.SipStack_isValid(swigCPtr);
    return ret;
  }

  public bool stop() {
    bool ret = tinyWRAPPINVOKE.SipStack_stop(swigCPtr);
    return ret;
  }

}
