/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2020 Huawei Technologies Co., Ltd.
 *   @Author: Julian Bouzas <julian.bouzas@collabora.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __GST_RTP_ELEMENTS_H__
#define __GST_RTP_ELEMENTS_H__

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_good/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

void rtp_element_init (GstPlugin * plugin);

GST_ELEMENT_REGISTER_DECLARE (rtpac3depay);
GST_ELEMENT_REGISTER_DECLARE (rtpac3pay);
GST_ELEMENT_REGISTER_DECLARE (rtpbvdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpbvpay);
GST_ELEMENT_REGISTER_DECLARE (rtpceltdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpceltpay);
GST_ELEMENT_REGISTER_DECLARE (rtpdvdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpdvpay);
GST_ELEMENT_REGISTER_DECLARE (rtpgstdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpgstpay);
GST_ELEMENT_REGISTER_DECLARE (rtpilbcpay);
GST_ELEMENT_REGISTER_DECLARE (rtpilbcdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpg722depay);
GST_ELEMENT_REGISTER_DECLARE (rtpg722pay);
GST_ELEMENT_REGISTER_DECLARE (rtpg723depay);
GST_ELEMENT_REGISTER_DECLARE (rtpg723pay);
GST_ELEMENT_REGISTER_DECLARE (rtpg726depay);
GST_ELEMENT_REGISTER_DECLARE (rtpg726pay);
GST_ELEMENT_REGISTER_DECLARE (rtpg729depay);
GST_ELEMENT_REGISTER_DECLARE (rtpg729pay);
GST_ELEMENT_REGISTER_DECLARE (rtpgsmdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpgsmpay);
GST_ELEMENT_REGISTER_DECLARE (rtpamrdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpamrpay);
GST_ELEMENT_REGISTER_DECLARE (rtppcmadepay);
GST_ELEMENT_REGISTER_DECLARE (rtppcmudepay);
GST_ELEMENT_REGISTER_DECLARE (rtppcmupay);
GST_ELEMENT_REGISTER_DECLARE (rtppcmapay);
GST_ELEMENT_REGISTER_DECLARE (rtpmpadepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmpapay);
GST_ELEMENT_REGISTER_DECLARE (rtpmparobustdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmpvdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmpvpay);
GST_ELEMENT_REGISTER_DECLARE (rtpopusdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpopuspay);
GST_ELEMENT_REGISTER_DECLARE (rtph261pay);
GST_ELEMENT_REGISTER_DECLARE (rtph261depay);
GST_ELEMENT_REGISTER_DECLARE (rtph263ppay);
GST_ELEMENT_REGISTER_DECLARE (rtph263pdepay);
GST_ELEMENT_REGISTER_DECLARE (rtph263depay);
GST_ELEMENT_REGISTER_DECLARE (rtph263pay);
GST_ELEMENT_REGISTER_DECLARE (rtph264depay);
GST_ELEMENT_REGISTER_DECLARE (rtph264pay);
GST_ELEMENT_REGISTER_DECLARE (rtph265depay);
GST_ELEMENT_REGISTER_DECLARE (rtph265pay);
GST_ELEMENT_REGISTER_DECLARE (rtpj2kdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpj2kpay);
GST_ELEMENT_REGISTER_DECLARE (rtpjpegdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpjpegpay);
GST_ELEMENT_REGISTER_DECLARE (rtpklvdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpklvpay);
GST_ELEMENT_REGISTER_DECLARE (rtpL8pay);
GST_ELEMENT_REGISTER_DECLARE (rtpL8depay);
GST_ELEMENT_REGISTER_DECLARE (rtpL16pay);
GST_ELEMENT_REGISTER_DECLARE (rtpL16depay);
GST_ELEMENT_REGISTER_DECLARE (rtpL24pay);
GST_ELEMENT_REGISTER_DECLARE (rtpL24depay);
GST_ELEMENT_REGISTER_DECLARE (rtpldacpay);
GST_ELEMENT_REGISTER_DECLARE (asteriskh263);
GST_ELEMENT_REGISTER_DECLARE (rtpmp1sdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp2tdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp2tpay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4vpay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4vdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4apay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4adepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4gdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpmp4gpay);
GST_ELEMENT_REGISTER_DECLARE (rtpqcelpdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpqdm2depay);
GST_ELEMENT_REGISTER_DECLARE (rtpsbcdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpsbcpay);
GST_ELEMENT_REGISTER_DECLARE (rtpsirenpay);
GST_ELEMENT_REGISTER_DECLARE (rtpsirendepay);
GST_ELEMENT_REGISTER_DECLARE (rtpspeexpay);
GST_ELEMENT_REGISTER_DECLARE (rtpspeexdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpsv3vdepay);
GST_ELEMENT_REGISTER_DECLARE (rtptheoradepay);
GST_ELEMENT_REGISTER_DECLARE (rtptheorapay);
GST_ELEMENT_REGISTER_DECLARE (rtpvorbisdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpvorbispay);
GST_ELEMENT_REGISTER_DECLARE (rtpvp8depay);
GST_ELEMENT_REGISTER_DECLARE (rtpvp8pay);
GST_ELEMENT_REGISTER_DECLARE (rtpvp9depay);
GST_ELEMENT_REGISTER_DECLARE (rtpvp9pay);
GST_ELEMENT_REGISTER_DECLARE (rtpvrawdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpvrawpay);
GST_ELEMENT_REGISTER_DECLARE (rtpstreampay);
GST_ELEMENT_REGISTER_DECLARE (rtpstreamdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpisacpay);
GST_ELEMENT_REGISTER_DECLARE (rtpisacdepay);
GST_ELEMENT_REGISTER_DECLARE (rtpredenc);
GST_ELEMENT_REGISTER_DECLARE (rtpreddec);
GST_ELEMENT_REGISTER_DECLARE (rtpulpfecdec);
GST_ELEMENT_REGISTER_DECLARE (rtpulpfecenc);
GST_ELEMENT_REGISTER_DECLARE (rtpstorage);
GST_ELEMENT_REGISTER_DECLARE (rtphdrextcolorspace);

G_END_DECLS

#endif /* __GST_RTP_ELEMENTS_H__ */
