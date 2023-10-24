// Страница "ОБНОВЛЕНИЕ" (правая панель)

import {h, Component} from "preact";
import Message from "../components/Message";

export default class NetworkManager extends Component {
	render() {
		return (
			<div style="padding: 0 1em;">
				<h2 className="tPan"><Message key="s.r.t"/></h2>
				<p className={"indent justify"}><Message key="s.r.c"/></p>
				<p className={"indent justify"}><Message key="s.r.c1"/></p>
				<p className={"justify"}><Message key="s.r.c2"/></p>
				<p className={"indent justify"}><Message key="s.r.c3"/></p>
			</div>
		);
	}
}
