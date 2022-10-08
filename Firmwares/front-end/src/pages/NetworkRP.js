// Страница "НАСТРОЙКИ" (правая панель)

import {h, Component} from "preact";
import Message from "../components/Message";

export default class Parameters extends Component {
	render() {
		return (
			<div style="padding: 0 1em;">
				<h2 className="tPan ltr"><Message key="n.r.t"/></h2>
				<p className={"indent justify"}><Message key="n.r.c"/></p>
				<p className={"indent justify"}><Message key="n.r.c1"/></p>
				<p className={"indent justify"}><Message key="n.r.c2"/></p>
				<p className={"indent justify"}><Message key="n.r.c3"/></p>
				<p className={"indent justify"}><Message key="n.r.c4"/></p>
			</div>
		);
	}
}