/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.tinyWRAP;

public enum tmedia_qos_stype_t {
  tmedia_qos_stype_none,
  tmedia_qos_stype_segmented,
  tmedia_qos_stype_e2e;

  public final int swigValue() {
    return swigValue;
  }

  public static tmedia_qos_stype_t swigToEnum(int swigValue) {
    tmedia_qos_stype_t[] swigValues = tmedia_qos_stype_t.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (tmedia_qos_stype_t swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + tmedia_qos_stype_t.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private tmedia_qos_stype_t() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private tmedia_qos_stype_t(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private tmedia_qos_stype_t(tmedia_qos_stype_t swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}

