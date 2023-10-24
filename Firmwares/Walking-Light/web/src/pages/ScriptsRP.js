// Страница "СЦЕНАРИИ" (правая панель)

import {h, Component} from "preact";
import Message from "../components/Message";

export default class NetworkManager extends Component {
	render() {
		return (
			<div style="padding: 0 1em;">
				<h2 className="tPan"><Message key="sc.r.t"/></h2>
				<p className={"indent justify"}><Message key="sc.r.c"/></p>
			</div>
		);
	}
}
