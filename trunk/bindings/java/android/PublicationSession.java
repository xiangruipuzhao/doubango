/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.tinyWRAP;

public class PublicationSession extends SipSession {
  private long swigCPtr;

  protected PublicationSession(long cPtr, boolean cMemoryOwn) {
    super(tinyWRAPJNI.SWIGPublicationSessionUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(PublicationSession obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        tinyWRAPJNI.delete_PublicationSession(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public boolean Publish(byte[] bytes) {
    if(bytes != null){
		final java.nio.ByteBuffer byteBuffer = this.getByteBuffer(bytes);
        return this.Publish(byteBuffer, bytes.length);
    }
    return false;
  }

  public PublicationSession(SipStack Stack) {
    this(tinyWRAPJNI.new_PublicationSession(SipStack.getCPtr(Stack), Stack), true);
  }

  public boolean Publish(java.nio.ByteBuffer payload, long len) {
    return tinyWRAPJNI.PublicationSession_Publish(swigCPtr, this, payload, len);
  }

  public boolean UnPublish() {
    return tinyWRAPJNI.PublicationSession_UnPublish(swigCPtr, this);
  }

}
