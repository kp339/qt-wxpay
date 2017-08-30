<?php
ini_set('date.timezone','Asia/Shanghai');
//error_reporting(E_ERROR);

require_once "../lib/WxPay.Api.php";
require_once "WxPay.NativePay.php";
require_once 'log.php';

$notify = new NativePay();

$input = new WxPayUnifiedOrder();
$input->SetBody("test");
$input->SetAttach("test");
// 保存订单号码
$order_num = WxPayConfig::MCHID.date("YmdHis");
$input->SetOut_trade_no($order_num);
// 获取输入金额
$money = $_POST['money'];
if (empty($money)) {
	$money = "1";
}
$input->SetTotal_fee($money);
$input->SetTime_start(date("YmdHis"));
$input->SetTime_expire(date("YmdHis", time() + 600));
$input->SetGoods_tag("test");
// 设置支付回调地址，这里直接使用微信自带的回调地址
$input->SetNotify_url("http://paysdk.weixin.qq.com/example/notify.php");
$input->SetTrade_type("NATIVE");
$input->SetProduct_id("123456789");
$result = $notify->GetPayUrl($input);
// 获取支付链接
$url = $result["code_url"];
if (!empty(url)) {
	// 使用###作为分隔符
	echo $order_num . "###" . $url;
}
?>